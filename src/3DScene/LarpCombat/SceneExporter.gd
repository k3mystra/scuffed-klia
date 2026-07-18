@tool
extends EditorScript

# Exports a flattened C++ scene. Run AutoAnimator first: its generated clips
# contain world-space transforms and are named after their Path3D route.
const OUTPUT_PATH = "res://world.txt"
const SOURCE_ANIMATION = "ScriptTest"
const PROJECT_OFFSET_FROM_SRC = "3DScene/LarpCombat/"

var scene_root: Node

func _run() -> void:
	var root = EditorInterface.get_edited_scene_root()
	if not root:
		push_error("Open a scene before exporting.")
		return
	scene_root = root
	var file = FileAccess.open(OUTPUT_PATH, FileAccess.WRITE)
	if not file:
		push_error("Could not create " + OUTPUT_PATH)
		return

	write_entities(root, file)
	write_animations(root, file)
	file.close()
	print("Exported scene and route animations to ", OUTPUT_PATH)

func write_entities(node: Node, file: FileAccess) -> void:
	if node is MeshInstance3D:
		write_mesh(node, file)
	elif node is Camera3D:
		write_camera(node, file)
	for child in node.get_children():
		write_entities(child, file)

func write_mesh(node: MeshInstance3D, file: FileAccess) -> void:
	file.store_string("# %s\n" % entity_id(node))
	file.store_string(transform_line(node.global_transform))
	var mesh_path := ""
	if node.mesh and node.mesh.resource_path.get_extension() == "obj":
		mesh_path = PROJECT_OFFSET_FROM_SRC + node.mesh.resource_path.trim_prefix("res://")
	file.store_string("M%s\n\n" % (" " + mesh_path if not mesh_path.is_empty() else ""))

func write_camera(node: Camera3D, file: FileAccess) -> void:
	file.store_string("# %s\n" % entity_id(node))
	file.store_string(transform_line(node.global_transform))
	file.store_string("C\n\n")

func transform_line(transform: Transform3D) -> String:
	var position := transform.origin
	var rotation := transform.basis.get_rotation_quaternion()
	var scale := transform.basis.get_scale()
	return "T %f %f %f %f %f %f %f %f %f %f\n" % [position.x, position.y, position.z, rotation.w, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z]

func write_animations(root: Node, file: FileAccess) -> void:
	var player := find_animation_player(root)
	if not player:
		push_warning("No AnimationPlayer found; no animations exported.")
		return
	var animation_root := player.get_node_or_null(player.root_node)
	if not animation_root:
		push_error("Could not resolve AnimationPlayer.root_node.")
		return

	for name in player.get_animation_list():
		var animation := player.get_animation(name)
		# Only AutoAnimator's per-Path3D clips belong in the runtime world. This
		# excludes the source progress tracks and legacy combined baked clips.
		if not animation.has_meta("route_animation"):
			continue
		write_animation(animation, name, animation_root, file)

func write_animation(animation: Animation, name: String, animation_root: Node, file: FileAccess) -> void:
	var tracks := ""
	for track_index in animation.get_track_count():
		var type_name := track_type_name(animation.track_get_type(track_index))
		if type_name.is_empty():
			continue
		var target = animation_root.get_node_or_null(animation.track_get_path(track_index))
		if not (target is MeshInstance3D):
			push_warning("Skipping non-mesh animation target: " + String(animation.track_get_path(track_index)))
			continue
		tracks += "Z %s %s %d " % [entity_id(target), type_name, animation.track_get_key_count(track_index)]
		for key_index in animation.track_get_key_count(track_index):
			tracks += key_string(type_name, animation.track_get_key_time(track_index, key_index), animation.track_get_key_value(track_index, key_index))
	if not tracks.is_empty():
		file.store_string("A %s %f %d %s\n" % [name, animation.length, animation.loop_mode, tracks])

func track_type_name(type: int) -> String:
	match type:
		Animation.TYPE_POSITION_3D: return "POS"
		Animation.TYPE_ROTATION_3D: return "ROT"
		Animation.TYPE_SCALE_3D: return "SCL"
	return ""

func key_string(type_name: String, time: float, value: Variant) -> String:
	if type_name == "ROT":
		return "K %f %f %f %f %f " % [time, value.w, value.x, value.y, value.z]
	return "K %f %f %f %f " % [time, value.x, value.y, value.z]

func entity_id(node: Node) -> String:
	# Use the scene-relative path. Animation tracks are resolved from the same
	# scene root, so this string is also what World.cpp uses for the lookup key.
	var id := String(scene_root.get_path_to(node))
	if id.contains(" ") or id.contains("\t"):
		push_error("Node paths with whitespace cannot be exported: " + id)
	return id

func find_animation_player(node: Node) -> AnimationPlayer:
	if node is AnimationPlayer:
		return node
	for child in node.get_children():
		var result := find_animation_player(child)
		if result:
			return result
	return null
