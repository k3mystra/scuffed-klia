@tool
extends EditorScript

# ==================== HOW TO USE ====================
# 1. Under any Path3D node, add a PathFollow3D child node, and under that
#    add your vehicle/mesh node.
# 2. In the AnimationPlayer, create an animation and add a track to animate
#    the "progress" or "progress_ratio" of the PathFollow3D node.
# 3. If you want to specify a reversing segment:
#    Add Metadata "end_of_reverse" (int) on the Path3D grandparent node,
#    specifying the index of the Curve3D point where the reversing ends.
#    All movement along the curve before this point will be automatically
#    reversed (rotated 180 degrees around Y).
# 4. If you want height constraints:
#    Add Metadata "min_y" (float) on the PathFollow3D or Path3D node.
# 5. Set INPUT_ANIMATION_NAME and OUTPUT_ANIMATION_NAME below.
# 6. Run this script (File > Run). It reads the progress tracks from the input
#    animation, bakes the corresponding 3D Position and 3D Rotation tracks on the
#    PathFollow3D node in the output animation, and disables the progress track
#    in the output animation if they are the same.
# =======================================================

const INPUT_ANIMATION_NAME = "ScriptTest"
const OUTPUT_ANIMATION_NAME = "ScriptTest_baked"
const SAMPLE_DELTA = 1.0                  # Time step (seconds) for keyframe baking

func _run():
	var scene_root = EditorInterface.get_edited_scene_root()
	if not scene_root:
		print("Error: open a scene first!")
		return

	var anim_player = find_in_tree(scene_root)
	if not anim_player:
		print("Error: no AnimationPlayer found anywhere in the scene")
		return

	var lib = anim_player.get_animation_library("")
	if not lib:
		lib = AnimationLibrary.new()
		anim_player.add_animation_library("", lib)

	if not lib.has_animation(INPUT_ANIMATION_NAME):
		print("Error: Input animation '", INPUT_ANIMATION_NAME, "' not found in library")
		return

	var input_anim = lib.get_animation(INPUT_ANIMATION_NAME)
	var output_anim: Animation

	if INPUT_ANIMATION_NAME == OUTPUT_ANIMATION_NAME:
		output_anim = input_anim
	else:
		if lib.has_animation(OUTPUT_ANIMATION_NAME):
			output_anim = lib.get_animation(OUTPUT_ANIMATION_NAME)
		else:
			output_anim = Animation.new()
			lib.add_animation(OUTPUT_ANIMATION_NAME, output_anim)
		
		# Match length to input animation
		output_anim.length = input_anim.length

	bake_path_follow_tracks(anim_player, input_anim, output_anim)
	print("\nDone. Baked '", OUTPUT_ANIMATION_NAME, "' (length ", output_anim.length, "s)")


func bake_path_follow_tracks(anim_player: AnimationPlayer, input_anim: Animation, output_anim: Animation):
	var root_node = anim_player.get_node(anim_player.root_node)
	if not root_node:
		print("Error: root_node of AnimationPlayer not found")
		return

	print("Animation tracks count: ", input_anim.get_track_count())
	# Collect all progress tracks to process from the input animation
	var tracks_to_process: Array[Dictionary] = []
	for i in range(input_anim.get_track_count()):
		var path_str = String(input_anim.track_get_path(i))
		var track_type = input_anim.track_get_type(i)
		print("Track ", i, ": path='", path_str, "', type=", track_type)
		if track_type == Animation.TYPE_VALUE or track_type == Animation.TYPE_BEZIER:
			if ":" in path_str:
				var parts = path_str.split(":")
				var node_path_str = parts[0]
				var property_name = parts[1]
				var node_path = NodePath(node_path_str)
				var path_follow = root_node.get_node_or_null(node_path)
				print("  -> property: '", property_name, "', resolved node: ", path_follow)
				if path_follow:
					print("  -> node class: ", path_follow.get_class())
				
				if property_name == "progress" or property_name == "progress_ratio":
					if path_follow and path_follow is PathFollow3D:
						tracks_to_process.append({
							"track_idx": i,
							"track_type": track_type,
							"node_path": node_path,
							"property_name": property_name,
							"path_follow": path_follow
						})

	if tracks_to_process.is_empty():
		print("No PathFollow3D progress/progress_ratio tracks found in input animation '", INPUT_ANIMATION_NAME, "'")
		return

	# Process in reverse order to keep track indices valid when editing in-place
	tracks_to_process.reverse()

	for info in tracks_to_process:
		var track_idx = info["track_idx"]
		var node_path = info["node_path"]
		var property_name = info["property_name"]
		var path_follow = info["path_follow"] as PathFollow3D
		var path_3d = path_follow.get_parent() as Path3D

		if not path_3d:
			print("Warning: Parent of PathFollow3D '", path_follow.name, "' is not a Path3D node. Skipping.")
			continue

		print("Baking path for '", path_follow.name, "' using grandparent '", path_3d.name, "'")

		# Calculate total curve length
		var curve = path_3d.curve
		if not curve:
			print("Warning: Path3D '", path_3d.name, "' has no Curve3D. Skipping.")
			continue
		var total_length = curve.get_baked_length()

		# Determine the reverse end distance if end_of_reverse metadata is defined
		var has_reverse = false
		var reverse_end_dist = 0.0
		if path_3d.has_meta("end_of_reverse"):
			var end_idx = int(path_3d.get_meta("end_of_reverse"))
			if end_idx >= 0 and end_idx < curve.point_count:
				var end_pos = curve.get_point_position(end_idx)
				reverse_end_dist = curve.get_closest_offset(end_pos)
				has_reverse = true
				print("   Reverse mode enabled. End of reverse point index: ", end_idx, " (offset: ", reverse_end_dist, " units)")
			else:
				print("   Warning: end_of_reverse index ", end_idx, " is out of bounds (0 to ", curve.point_count - 1, ").")

		# Fetch min_y constraint
		var min_y = -INF
		if path_follow.has_meta("min_y"):
			min_y = path_follow.get_meta("min_y")
		elif path_3d.has_meta("min_y"):
			min_y = path_3d.get_meta("min_y")

		# Collect times to sample from input_anim
		var times: Array[float] = []
		var key_count = input_anim.track_get_key_count(track_idx)
		for k in range(key_count):
			times.append(input_anim.track_get_key_time(track_idx, k))

		var t_sample = 0.0
		while t_sample < input_anim.length:
			times.append(t_sample)
			t_sample += SAMPLE_DELTA
		times.append(input_anim.length)

		times.sort()

		# Deduplicate times
		var unique_times: Array[float] = []
		for time in times:
			if unique_times.is_empty():
				unique_times.append(time)
			else:
				if time - unique_times[-1] > 0.0001:
					unique_times.append(time)

		var track_type = info["track_type"]

		# Store original value to restore it in the editor afterwards
		var original_val = path_follow.get(property_name)

		# Get start transform of the PathFollow3D node (T_start) at the beginning of the animation (time = 0.0)
		var start_val = 0.0
		if track_type == Animation.TYPE_VALUE:
			start_val = input_anim.value_track_interpolate(track_idx, 0.0)
		elif track_type == Animation.TYPE_BEZIER:
			start_val = input_anim.bezier_track_interpolate(track_idx, 0.0)
		path_follow.set(property_name, start_val)
		var T_start = path_follow.transform

		# Find the child node that is the mesh node
		var child = path_follow.get_child(0) as Node3D
		var T_child_design = Transform3D.IDENTITY
		var target_path = node_path
		
		if child:
			T_child_design = child.transform
			target_path = NodePath(String(node_path) + "/" + child.name)
			print("   Target redirected to child mesh node: '", child.name, "' (path: ", target_path, ")")

		# Sample transforms
		var positions: Array[Vector3] = []
		var rotations: Array[Quaternion] = []

		for time in unique_times:
			var val = 0.0
			if track_type == Animation.TYPE_VALUE:
				val = input_anim.value_track_interpolate(track_idx, time)
			elif track_type == Animation.TYPE_BEZIER:
				val = input_anim.bezier_track_interpolate(track_idx, time)
			path_follow.set(property_name, val)
			
			var T_follow = path_follow.transform

			# Apply reverse logic based on distance along curve
			var current_dist = val
			if property_name == "progress_ratio":
				current_dist = val * total_length

			var is_reversing = has_reverse and current_dist < reverse_end_dist
			var euler_before = T_follow.basis.get_rotation_quaternion().get_euler()

			# Since the model naturally faces backwards (towards the curve's trails),
			# if we have a reverse segment, we need to flip (offset) the rotation by 180 degrees
			# during the forward phase, and keep it unflipped during the reversing phase.
			var should_flip = has_reverse and not is_reversing

			if should_flip:
				# Rotate 180 degrees around local Y axis
				var reverse_rot = Quaternion(Vector3.UP, PI)
				T_follow.basis = T_follow.basis * Basis(reverse_rot)

			# Apply min_y clamping if specified
			if min_y != -INF and T_follow.origin.y < min_y:
				T_follow.origin.y = min_y

			# Calculate child's final local transform relative to PathFollow3D's starting state
			var pos: Vector3
			var rot: Quaternion
			if child:
				var T_child_final = T_start.inverse() * T_follow * T_child_design
				pos = T_child_final.origin
				rot = T_child_final.basis.get_rotation_quaternion()
			else:
				pos = T_follow.origin
				rot = T_follow.basis.get_rotation_quaternion()

			var euler_after = rot.get_euler()
			print("   Time: ", time, " | val: ", val, " | is_reversing: ", is_reversing, " | should_flip: ", should_flip, " | Euler Before (deg): ", rad_to_deg(euler_before.y), " | Euler After (deg): ", rad_to_deg(euler_after.y))

			positions.append(pos)
			rotations.append(rot)

		# Restore original value
		path_follow.set(property_name, original_val)

		# Remove any existing position/rotation tracks for this target node path in output_anim
		var k = output_anim.get_track_count() - 1
		while k >= 0:
			var t_path = output_anim.track_get_path(k)
			var t_type = output_anim.track_get_type(k)
			if t_path == target_path and (t_type == Animation.TYPE_POSITION_3D or t_type == Animation.TYPE_ROTATION_3D):
				output_anim.remove_track(k)
			k -= 1

		# Create new position and rotation tracks in output_anim
		var pos_track = output_anim.add_track(Animation.TYPE_POSITION_3D)
		output_anim.track_set_path(pos_track, target_path)
		var rot_track = output_anim.add_track(Animation.TYPE_ROTATION_3D)
		output_anim.track_set_path(rot_track, target_path)

		# Write keys to tracks in output_anim
		for idx in range(unique_times.size()):
			var time = unique_times[idx]
			output_anim.track_insert_key(pos_track, time, positions[idx])
			output_anim.track_insert_key(rot_track, time, rotations[idx])

		# Disable original track in output_anim (or input_anim if in-place) to prevent double-animation
		if input_anim == output_anim:
			output_anim.track_set_enabled(track_idx, false)
		print("   Successfully baked ", unique_times.size(), " keyframes onto '", path_follow.name, "'")


func find_in_tree(node: Node) -> AnimationPlayer:
	if node is AnimationPlayer:
		return node
	for child in node.get_children():
		var result = find_in_tree(child)
		if result:
			return result
	return null
