@tool
extends EditorScript

# Set the path where your C++ engine expects to read the layout configuration
const OUTPUT_PATH = "res://scene_layout.txt"

func _run():
	var root = get_editor_interface().get_edited_scene_root()
	if not root:
		print("Error: Open a scene first!")
		return
		
	var file = FileAccess.open(OUTPUT_PATH, FileAccess.WRITE)
	if not file:
		print("Error: Could not create output file!")
		return
		
	print("Starting export for scene: ", root.name)
	parse_node(root, file)
	file.close()
	print("Export complete! Saved to: ", OUTPUT_PATH)

func parse_node(node: Node, file: FileAccess):
	if node is MeshInstance3D:
		if node.mesh and node.mesh.resource_path.get_extension() == "obj":
			# 1. Get the local clean path to the obj asset
			var obj_path = node.mesh.resource_path.trim_prefix("res://")
			obj_path = "3DScene/LarpCombat/" + obj_path
			# 2. Extract Transform properties
			var pos = node.position
			var rot = node.rotation
			var scl = node.scale
			
			# 3. Format line: path pos.x pos.y pos.z rot.x rot.y rot.z scl.x scl.y scl.z
			var line = "%s %f %f %f %f %f %f %f %f %f\n" % [
				obj_path,
				pos.x, pos.y, pos.z,
				rad_to_deg(rot.x), rad_to_deg(rot.y), rad_to_deg(rot.z), # Converts radians to degrees!
				scl.x, scl.y, scl.z
			]
			file.store_string(line)
			
	# Recursively check children to support nested nodes/groups
	for child in node.get_children():
		parse_node(child, file)
