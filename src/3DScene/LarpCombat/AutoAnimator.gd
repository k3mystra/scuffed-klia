@tool
extends EditorScript

# Bakes the progress/progress_ratio tracks in SOURCE_ANIMATION into one clip
# per Path3D. Each generated clip is named after that Path3D. A Path3D may
# contain several PathFollow3D children (the baggage truck and its carts); all
# of them are baked into the same clip.
#
# The C++ renderer has a flat scene graph, so generated keys deliberately hold
# world-space position/rotation values. These clips are export-only; play the
# source animation in Godot if you want to preview it there.

const SOURCE_ANIMATION = "ScriptTest"
const SAMPLE_DELTA = 1.0
const FADE_IN_DURATION = 2.0
const FADE_OUT_DURATION = 2.0

func _run() -> void:
	var scene_root = EditorInterface.get_edited_scene_root()
	if not scene_root:
		push_error("Open the airport scene before running AutoAnimator.")
		return

	var player = find_animation_player(scene_root)
	if not player:
		push_error("No AnimationPlayer exists in the open scene.")
		return

	var library = player.get_animation_library("")
	if not library or not library.has_animation(SOURCE_ANIMATION):
		push_error("Animation '%s' was not found." % SOURCE_ANIMATION)
		return

	var source = library.get_animation(SOURCE_ANIMATION)
	var animation_root = player.get_node_or_null(player.root_node)
	if not animation_root:
		push_error("Could not resolve AnimationPlayer.root_node.")
		return

	var routes := collect_route_tracks(source, animation_root)
	if routes.is_empty():
		push_warning("No PathFollow3D progress tracks were found in '%s'." % SOURCE_ANIMATION)
		return

	for route_name in routes:
		bake_route(library, route_name, routes[route_name], source, animation_root, scene_root)

	print("Baked ", routes.size(), " route animations from '", SOURCE_ANIMATION, "'.")

func collect_route_tracks(source: Animation, animation_root: Node) -> Dictionary:
	var routes := {}
	for track_index in source.get_track_count():
		var property_path := String(source.track_get_path(track_index))
		var track_type := source.track_get_type(track_index)
		if track_type != Animation.TYPE_VALUE and track_type != Animation.TYPE_BEZIER:
			continue

		var separator := property_path.find(":")
		if separator < 0:
			continue
		var property_name := property_path.substr(separator + 1)
		if property_name != "progress" and property_name != "progress_ratio":
			continue

		var follow = animation_root.get_node_or_null(NodePath(property_path.substr(0, separator))) as PathFollow3D
		if not follow or not follow.get_parent() is Path3D:
			continue
		var route = follow.get_parent() as Path3D
		if routes.has(route.name) and routes[route.name]["route"] != route:
			push_error("Path3D name '%s' is not unique; route animation names must be unique." % route.name)
			continue
		if not routes.has(route.name):
			routes[route.name] = {"route": route, "tracks": []}
		routes[route.name]["tracks"].append({
			"index": track_index,
			"type": track_type,
			"property": property_name,
			"follow": follow
		})
	return routes

func get_scene_relative_transform(node: Node, scene_root: Node) -> Transform3D:
	var t := Transform3D.IDENTITY
	var curr := node
	while curr and curr != scene_root:
		if curr is Node3D:
			t = curr.transform * t
		curr = curr.get_parent()
	return t

func bake_route(library: AnimationLibrary, route_name: String, route_info: Dictionary, source: Animation, animation_root: Node, scene_root: Node) -> void:
	var is_arrival := route_name.to_lower().contains("arrival")
	var is_departure := route_name.to_lower().contains("departure")
	print("AutoAnimator baking: ", route_name, " (is_arrival=", is_arrival, ", is_departure=", is_departure, ")")

	if library.has_animation(route_name):
		library.remove_animation(route_name)

	var output := Animation.new()
	output.loop_mode = source.loop_mode
	output.set_meta("route_animation", true)
	library.add_animation(route_name, output)

	var max_duration := 0.0
	for track_info in route_info["tracks"]:
		var duration := bake_path_follow_track(output, source, route_info["route"], track_info, animation_root, scene_root, route_name)
		if duration > max_duration:
			max_duration = duration

	if max_duration <= 0.0:
		max_duration = source.length
	output.length = max_duration

	print("  ", route_name, ": ", route_info["tracks"].size(), " PathFollow3D track(s), duration: ", max_duration)

func bake_path_follow_track(output: Animation, source: Animation, route: Path3D, track_info: Dictionary, animation_root: Node, scene_root: Node, route_name: String) -> float:
	var follow := track_info["follow"] as PathFollow3D
	var target := first_mesh_child(follow)
	if not target:
		push_warning("Skipping '%s': it has no MeshInstance3D child." % follow.get_path())
		return 0.0

	var times := sample_times(source, track_info["index"])
	var original_value = follow.get(track_info["property"])
	# Preserve the original baker's reference frame: generated mesh keys are
	# relative to the PathFollow3D state at time zero, not route world-space.
	var start_value := interpolated_value(source, track_info, 0.0)
	follow.set(track_info["property"], start_value)
	var follow_at_start := follow.transform
	var mesh_design_transform := target.transform
	# C++ has no parent hierarchy. This is the world transform represented by
	# the old baker's local reference frame at time zero.
	var route_transform := get_scene_relative_transform(route, scene_root)
	var start_frame_in_world := route_transform * follow_at_start
	var positions: Array[Vector3] = []
	var rotations: Array[Quaternion] = []
	var reverse_end_distance := reverse_end_distance_for(route)
	var min_y := minimum_y_for(route, follow)
	var curve_length := route.curve.get_baked_length() if route.curve else 0.0

	for time in times:
		var progress_value := interpolated_value(source, track_info, time)
		follow.set(track_info["property"], progress_value)
		var transform := follow.transform

		# Preserve the original reverse convention: before end_of_reverse the
		# vehicle is reversing, while the remainder faces the opposite direction.
		# The 180-degree turn is applied about the vehicle's local Y axis.
		var distance := progress_value * curve_length if track_info["property"] == "progress_ratio" else progress_value
		var is_reversing := reverse_end_distance >= 0.0 and distance < reverse_end_distance
		if reverse_end_distance >= 0.0 and not is_reversing:
			transform.basis = transform.basis * Basis(Quaternion(Vector3.UP, PI))

		if min_y != -INF and transform.origin.y < min_y:
			transform.origin.y = min_y

		# Preserve the original start-relative calculation, then convert it to
		# world space for the flattened C++ scene. At t = 0 this exactly equals
		# target.global_transform, so the animation cannot jump on its first key.
		var mesh_relative_to_start := follow_at_start.inverse() * transform * mesh_design_transform
		var mesh_transform := start_frame_in_world * mesh_relative_to_start
		positions.append(mesh_transform.origin)
		rotations.append(mesh_transform.basis.get_rotation_quaternion())

	follow.set(track_info["property"], original_value)

	# Find when the track actually stops moving
	var end_index := times.size() - 1
	while end_index > 0:
		var pos_diff := (positions[end_index] - positions[end_index - 1]).length()
		var rot_diff := rotations[end_index].dot(rotations[end_index - 1])
		if pos_diff > 0.001 or rot_diff < 0.9999:
			break
		end_index -= 1
	var track_duration := times[end_index]

	# Truncate keyframes to only include active movement
	times = times.slice(0, end_index + 1)
	positions = positions.slice(0, end_index + 1)
	rotations = rotations.slice(0, end_index + 1)

	# Generate transparency values
	var transparencies: Array[float] = []
	var is_arrival := route_name.to_lower().contains("arrival")
	var is_departure := route_name.to_lower().contains("departure")
	for time in times:
		var transparency := 0.0
		if is_arrival:
			if time < FADE_IN_DURATION:
				transparency = 1.0 - (time / FADE_IN_DURATION)
		elif is_departure:
			if time > track_duration - FADE_OUT_DURATION:
				var remaining_time := track_duration - time
				if FADE_OUT_DURATION > 0.0:
					transparency = 1.0 - clamp(remaining_time / FADE_OUT_DURATION, 0.0, 1.0)
				else:
					transparency = 1.0
		else:
			if time < FADE_IN_DURATION:
				transparency = 1.0 - (time / FADE_IN_DURATION)
			elif time > track_duration - FADE_OUT_DURATION:
				var remaining_time := track_duration - time
				if FADE_OUT_DURATION > 0.0:
					transparency = 1.0 - clamp(remaining_time / FADE_OUT_DURATION, 0.0, 1.0)
				else:
					transparency = 1.0
		transparency = clamp(transparency, 0.0, 1.0)
		transparencies.append(transparency)

	var target_path := animation_root.get_path_to(target)
	var pos_track := output.add_track(Animation.TYPE_POSITION_3D)
	output.track_set_path(pos_track, target_path)
	var rot_track := output.add_track(Animation.TYPE_ROTATION_3D)
	output.track_set_path(rot_track, target_path)
	for key_index in times.size():
		output.track_insert_key(pos_track, times[key_index], positions[key_index])
		output.track_insert_key(rot_track, times[key_index], rotations[key_index])

	# Add transparency track
	var trans_track := output.add_track(Animation.TYPE_VALUE)
	output.track_set_path(trans_track, "%s:transparency" % target_path)
	for key_index in times.size():
		output.track_insert_key(trans_track, times[key_index], transparencies[key_index])

	return track_duration

func reverse_end_distance_for(route: Path3D) -> float:
	if not route.has_meta("end_of_reverse") or not route.curve:
		return -1.0
	var point_index := int(route.get_meta("end_of_reverse"))
	if point_index < 0 or point_index >= route.curve.point_count:
		push_warning("Invalid end_of_reverse on " + String(route.get_path()))
		return -1.0
	return route.curve.get_closest_offset(route.curve.get_point_position(point_index))

func minimum_y_for(route: Path3D, follow: PathFollow3D) -> float:
	if follow.has_meta("min_y"):
		return float(follow.get_meta("min_y"))
	if route.has_meta("min_y"):
		return float(route.get_meta("min_y"))
	return -INF

func sample_times(source: Animation, track_index: int) -> Array[float]:
	var times: Array[float] = []
	for key_index in source.track_get_key_count(track_index):
		times.append(source.track_get_key_time(track_index, key_index))
	var time := 0.0
	while time < source.length:
		times.append(time)
		time += SAMPLE_DELTA
	times.append(source.length)
	times.sort()

	var unique_times: Array[float] = []
	for sample in times:
		if unique_times.is_empty() or sample - unique_times.back() > 0.0001:
			unique_times.append(sample)
	return unique_times

func interpolated_value(source: Animation, track_info: Dictionary, time: float) -> float:
	if track_info["type"] == Animation.TYPE_BEZIER:
		return source.bezier_track_interpolate(track_info["index"], time)
	return source.value_track_interpolate(track_info["index"], time)

func first_mesh_child(node: Node) -> MeshInstance3D:
	for child in node.get_children():
		if child is MeshInstance3D:
			return child
	return null

func find_animation_player(node: Node) -> AnimationPlayer:
	if node is AnimationPlayer:
		return node
	for child in node.get_children():
		var result := find_animation_player(child)
		if result:
			return result
	return null
