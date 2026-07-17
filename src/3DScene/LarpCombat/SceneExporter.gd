@tool
extends EditorScript

# Set the path where your C++ engine expects to read the layout configuration
const OUTPUT_PATH = "res://world.txt"
const ENTITY_DATA_PREFIX = "#"
const TRANSFORM_PREFIX = "T"
const MODEL_PREFIX = "M"
const CAMERA_PREFIX = "C"
const ANIMATION_PREFIX = "A"      # marks the start of an animation clip block
const TRACK_PREFIX = "Z"         # marks a track within a clip (which entity + property)
const KEYFRAME_PREFIX = "K"       # one keyframe: time + value

func _run():
	var root = EditorInterface.get_edited_scene_root()
	if not root:
		print("Error: Open a scene first!")
		return

	var file = FileAccess.open(OUTPUT_PATH, FileAccess.WRITE)
	if not file:
		print("Error: Could not create output file!")
		return

	print("Starting export for scene: ", root.name)
	parse_entities(root, file)
	file.close()
	print("Export complete! Saved to: ", OUTPUT_PATH)

func parse_entities(node: Node, file: FileAccess):
	var entityStr = ""
	if node is MeshInstance3D:
		entityStr = parse_3d_obj(node)
	elif node is Camera3D:
		entityStr = parse_cam(node)
	elif node is AnimationPlayer:
		entityStr = parse_anim_player(node)
	file.store_string(entityStr)

	# Recursively check children to support nested nodes/groups
	for child in node.get_children():
		parse_entities(child, file)

func parse_3d_obj(node: MeshInstance3D) -> String:
	var entityStr = ""
	entityStr += parse_entity_data_comp(node)
	entityStr += parse_transform_comp(node)
	entityStr += parse_model_comp(node)
	return entityStr + "\n"

func parse_cam(node: Camera3D):
	var entityStr = ""
	entityStr += parse_entity_data_comp(node)
	entityStr += parse_transform_comp(node)
	entityStr += parse_camera_comp(node)
	return entityStr + "\n"

func parse_transform_comp(node: Node3D) -> String:
	var pos = node.position
	var rot = node.quaternion
	var scl = node.scale

	# 3. Format line: path pos.x pos.y pos.z rot.w rot.x rot.y rot.z scl.x scl.y scl.z
	var line = TRANSFORM_PREFIX + " "
	line += "%f %f %f " % [pos.x, pos.y, pos.z]
	line += "%f %f %f %f " % [rot.w, rot.x, rot.y, rot.z]
	line += "%f %f %f\n" % [scl.x, scl.y, scl.z]
	return line

func parse_entity_data_comp(node: Node) -> String:
	return ENTITY_DATA_PREFIX + " " + node.name + "\n"

# Offset from your shared "src" folder (where main.cpp lives) down to this
# Godot project's own root. Since this matches the repo's folder structure,
# it's identical for every teammate regardless of where they cloned the repo.
const PROJECT_OFFSET_FROM_SRC = "3DScene/LarpCombat/"

func parse_model_comp(node: MeshInstance3D) -> String:
	var line = MODEL_PREFIX
	if not node.mesh or node.mesh.resource_path.get_extension() != "obj":
		return line + "\n"
	# Path relative to the shared "src" folder, not to this machine's
	# absolute filesystem, and not to Godot's own project root either
	# (since main.cpp lives one level above the Godot project).
	var relative_path = PROJECT_OFFSET_FROM_SRC + node.mesh.resource_path.trim_prefix("res://")
	line += " " + relative_path + "\n"
	return line

func parse_camera_comp(node: Camera3D) -> String:
	return CAMERA_PREFIX + "\n"

# ---- Animation export ----
#
# AnimationPlayer tracks reference OTHER nodes by NodePath, not itself, so
# we resolve each track's target here and emit the target's NODE NAME
# (matching the "#" entity name used elsewhere) so the C++ side can attach
# the clip to the correct entity without doing any path resolution itself.
#
# Only POSITION_3D / ROTATION_3D / SCALE_3D tracks are handled here, since
# that covers rigid-body motion (taxiing, pushback, gear, doors, etc).
# Skinned/bone animation is intentionally NOT handled -- your assets are
# static meshes, not armatures, so there is nothing to support there yet.
func parse_anim_player(node: AnimationPlayer) -> String:
	var out = parse_entity_data_comp(node)

	# Track paths are resolved relative to the player's root_node (usually
	# its parent, ".."), NOT relative to the AnimationPlayer node itself.
	var anim_root = node.get_node_or_null(node.root_node)
	if not anim_root:
		print("Warning: could not resolve root_node for AnimationPlayer: ", node.name)
		return out

	for anim_name in node.get_animation_list():
		if anim_name == "RESET":
			continue
		var anim: Animation = node.get_animation(anim_name)
		out += ANIMATION_PREFIX + " " + anim_name + " %f %d " % [anim.length, anim.loop_mode]

		for track_idx in range(anim.get_track_count()):
			var track_type = anim.track_get_type(track_idx)
			var track_path = anim.track_get_path(track_idx)
			var track_type_str = ""

			if track_type == Animation.TYPE_POSITION_3D:
				track_type_str = "POS"
			elif track_type == Animation.TYPE_ROTATION_3D:
				track_type_str = "ROT"
			elif track_type == Animation.TYPE_SCALE_3D:
				track_type_str = "SCL"
			elif track_type == Animation.TYPE_VALUE:
				# Generic property track, e.g. "NodePath:position" -- this is
				# what Godot creates when you keyframe a single Inspector
				# field directly instead of using "Insert Transform Track".
				var prop_name = String(track_path).get_slice(":", 1)
				match prop_name:
					"position": track_type_str = "POS"
					"rotation": track_type_str = "ROT_EULER"
					"quaternion": track_type_str = "ROT"
					"scale": track_type_str = "SCL"
					"transparency": track_type_str = "OPACITY"
					_:
						continue  # not a property we handle
			else:
				continue

			var target_node = anim_root.get_node_or_null(track_path)
			if not target_node:
				print("Warning: could not resolve animation track target: ", track_path)
				continue

			out += TRACK_PREFIX + " " + target_node.name + " " + track_type_str + " %d " % anim.track_get_key_count(track_idx)

			for key_idx in range(anim.track_get_key_count(track_idx)):
				var time = anim.track_get_key_time(track_idx, key_idx)
				var value = anim.track_get_key_value(track_idx, key_idx)
				out += KEYFRAME_PREFIX + " " + format_key_value(track_type_str, time, value)

		out += "\n"

	return out + "\n"

func format_key_value(track_type_str: String, time: float, value) -> String:
	match track_type_str:
		"POS", "SCL":
			return "%f %f %f %f " % [time, value.x, value.y, value.z]
		"ROT":
			# Quaternion (from a dedicated ROTATION_3D track, or a Value
			# track targeting the "quaternion" property)
			return "%f %f %f %f %f " % [time, value.w, value.x, value.y, value.z]
		"ROT_EULER":
			# Euler angles in radians (from a Value track targeting
			# "rotation" directly) -- C++ side should convert to quaternion
			# on load if it needs to combine with other rotation data.
			return "%f %f %f %f " % [time, value.x, value.y, value.z]
		"OPACITY":
			# Single float, 0 = fully opaque, 1 = fully invisible
			# (GeometryInstance3D.transparency)
			return "%f %f " % [time, value]
	return "%f\n" % time
