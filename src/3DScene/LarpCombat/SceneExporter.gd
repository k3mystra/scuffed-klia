@tool
extends EditorScript

# Set the path where your C++ engine expects to read the layout configuration
const OUTPUT_PATH = "res://world.txt"

const ENTITY_DATA_PREFIX = "#"
const TRANSFORM_PREFIX = "T"
const MODEL_PREFIX = "M"
const CAMERA_PREFIX = "C"

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
	var entityStr = "";
	if node is MeshInstance3D:
		entityStr = parse_3d_obj(node)
	elif node is Camera3D:
		entityStr = parse_cam(node)

	file.store_string(entityStr)
			
	# Recursively check children to support nested nodes/groups
	for child in node.get_children():
		parse_entities(child, file)


func parse_3d_obj(node: MeshInstance3D) -> String:
	var entityStr = "";
	entityStr += parse_entity_data_comp(node);
	entityStr += parse_transform_comp(node);
	entityStr += parse_model_comp(node);

	return entityStr + "\n";


func parse_cam(node: Camera3D):
	var entityStr = "";
	entityStr += parse_entity_data_comp(node);
	entityStr += parse_transform_comp(node);
	entityStr += parse_camera_comp(node);

	return entityStr;


func parse_transform_comp(node: Node3D) -> String:
	var pos = node.position
	var rot = node.quaternion
	var scl = node.scale
	
	# 3. Format line: path pos.x pos.y pos.z rot.w rot.x rot.y rot.z scl.x scl.y scl.z
	var line = TRANSFORM_PREFIX + " "
	line += "%f %f %f " % [pos.x, pos.y, pos.z]
	line += "%f %f %f %f " % [rot.w, rot.x, rot.y, rot.z]
	line += "%f %f %f\n" % [scl.x, scl.y, scl.z]

	return line;


func parse_entity_data_comp(node: Node) -> String:
	return ENTITY_DATA_PREFIX + " " + node.name + "\n"


func parse_model_comp(node: MeshInstance3D) -> String:
	var line = MODEL_PREFIX

	if not node.mesh or node.mesh.resource_path.get_extension() != "obj":
		return line + "\n";

	line += " " + ProjectSettings.globalize_path(node.mesh.resource_path) + "\n"
	return line;


func parse_camera_comp(node: Camera3D) -> String:
	return CAMERA_PREFIX + "\n"
