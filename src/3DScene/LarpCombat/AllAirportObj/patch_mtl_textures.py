"""
Run this AFTER the Blender export is done. This is a plain Python script --
run it from a normal terminal (not inside Blender), e.g.:

    python patch_mtl_textures.py

It walks every .mtl in the export folder and, for each material block that
matches an entry in MATERIAL_TEXTURES, inserts a "map_Kd <relative path>"
line right after the Kd line (if one isn't already there).
"""

import os
import glob

# ==================== CONFIGURATION ====================
export_folder = r"C:\Users\tan yu kai\Documents\GitHub\scuffed-klia\src\AllObjFiles"

# Material name -> texture path relative to each .mtl file's own location.
# Extend this dict if you find more materials with real textures later.
MATERIAL_TEXTURES = {
    "AIRPORT": "textures/Image_0.png",
    # "COLLIDERS" has no texture, so it's intentionally left out.
}
# =======================================================

mtl_files = glob.glob(os.path.join(export_folder, "*.mtl"))
print(f"Found {len(mtl_files)} .mtl files")
print(f"Looking for these material names: {list(MATERIAL_TEXTURES.keys())}\n")

patched_count = 0
all_materials_seen = set()

for mtl_path in mtl_files:
    with open(mtl_path, "r") as f:
        lines = f.readlines()

    current_material = None
    changed = False
    output_lines = []
    already_has_map_kd_for_current = False
    materials_in_this_file = []

    for line in lines:
        stripped = line.strip()

        if stripped.startswith("newmtl "):
            current_material = stripped.split(" ", 1)[1].strip()
            materials_in_this_file.append(current_material)
            all_materials_seen.add(current_material)
            already_has_map_kd_for_current = False
            output_lines.append(line)
            continue

        if stripped.startswith("map_Kd"):
            already_has_map_kd_for_current = True
            output_lines.append(line)
            continue

        # After the Kd line, insert map_Kd if this material needs one
        # and doesn't already have one.
        if stripped.startswith("Kd ") and current_material in MATERIAL_TEXTURES:
            output_lines.append(line)
            if not already_has_map_kd_for_current:
                tex_path = MATERIAL_TEXTURES[current_material]
                output_lines.append(f"map_Kd {tex_path}\n")
                already_has_map_kd_for_current = True
                changed = True
            continue

        output_lines.append(line)

    print(f"{os.path.basename(mtl_path)}: materials={materials_in_this_file} -> {'PATCHED' if changed else 'no match'}")

    if changed:
        with open(mtl_path, "w") as f:
            f.writelines(output_lines)
        patched_count += 1

print(f"\nAll unique material names seen across all files: {sorted(all_materials_seen)}")
print(f"Done. Patched {patched_count} of {len(mtl_files)} .mtl files.")