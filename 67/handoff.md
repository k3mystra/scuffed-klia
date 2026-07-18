# Handoff: C++ Animation System and Godot Exporter Alignment

## Table of Contents

- [Session Metadata](#session-metadata)
- [Current State Summary](#current-state-summary)
- [Codebase Understanding](#codebase-understanding)
  - [Architecture Overview](#architecture-overview)
  - [Critical Files](#critical-files)
- [Work Completed](#work-completed)
  - [Tasks Finished](#tasks-finished)
  - [Decisions & Implementations](#decisions--implementations)
- [Context for Resuming Agent](#context-for-resuming-agent)
  - [Important Context](#important-context)
  - [Assumptions Made](#assumptions-made)
  - [Potential Gotchas](#potential-gotchas)

---

## Session Metadata
- Updated: 2026-07-18T18:25:00+08:00
- Project: `c:\Users\tan yu kai\Documents\GitHub\scuffed-klia`
- Branch: `Jason`

## Current State Summary

The animation rendering pipeline has been fully implemented and aligned between Godot and the C++ application. The program compiles cleanly, controls smoothly without camera roll, and plays all route animations starting from their correct positions with correct model mappings.

---

## Codebase Understanding

### Architecture Overview

1. **Godot Editor constraints baking**:
   - `AutoAnimator.gd` samples Path3D curve progress and transforms them into absolute mesh local tracks.
   - `SceneExporter.gd` serializes static entities and animation clips to `world.txt`.
2. **C++ Game Engine**:
   - **Parser**: `World.cpp` parses the layout and animation tracks.
   - **Animation System**: `Animation.cpp` schedules clips, interpolates keys, and updates entity `Transform` vectors.
   - **Camera System**: `CameraControl.cpp` manages first-person panning using absolute yaw/pitch angles.

### Critical Files

| File | Purpose | Key Details |
|------|---------|-------------|
| [AutoAnimator.gd](file:///c:/Users/tan%20yu%20kai/Documents/GitHub/scuffed-klia/src/3DScene/LarpCombat/AutoAnimator.gd) | Baking tool script | Replaces `global_transform` with manual ancestor multiplication to bypass Godot's lazy viewport update in editor scripts. |
| [SceneExporter.gd](file:///c:/Users/tan%20yu%20kai/Documents/GitHub/scuffed-klia/src/3DScene/LarpCombat/SceneExporter.gd) | Layout and animation exporter | Exports parsed scene structures and animation keyframe tracks to `world.txt`. |
| [World.cpp](file:///c:/Users/tan%20yu%20kai/Documents/GitHub/scuffed-klia/src/World.cpp) | Resource Loader | Parses `world.txt` and aligns model, transform, and entity name mapping. |
| [Animation.cpp](file:///c:/Users/tan%20yu%20kai/Documents/GitHub/scuffed-klia/src/Animation.cpp) | Animation System | Handles `LOOP_LINEAR` and `LOOP_PINGPONG` interpolation (LERP for POS/SCL, SLERP for ROT). |
| [CameraControl.cpp](file:///c:/Users/tan%20yu%20kai/Documents/GitHub/scuffed-klia/src/CameraControl.cpp) | Camera Control | Implements yaw/pitch camera angles to prevent camera roll. |

---

## Work Completed

### Tasks Finished

- [x] **C++ Animation System**: Implemented track searching, time modulations (`fmod` loops and ping-pongs), and LERP/SLERP keyframe data math.
- [x] **Roll-Free Camera Panning**: Refactored first-person rotation to use pitch/yaw integration instead of local axis quat concatenation, preventing horizontal line tilting. Added pitch limits at $\pm89^\circ$.
- [x] **Escape & Cursor Lock Panning Fixes**: 
  - Mouse seeding is now done on initialization to prevent initial delta jumps.
  - Added `InputManager::resetMousePos` called when re-locking the mouse to prevent garbage deltas on window re-entry.
  - Disabled camera movements while the mouse cursor is unlocked.
- [x] **Baking Start Coordinates Fix**: Resolved the viewport coordinate bug where the first keyframe snapped to `(0,0,0)`. Used manual scene-relative transform calculation.
- [x] **Entity Mismatch Fix**: Fixed off-by-one mapping error in `World.cpp` where `#` parsing used `world.totalEntity - 1` before it was incremented. This maps tracks and models to their correct entities.

### Decisions & Implementations

1. **Interpolation Lookup**:
   `searchForNextKeyframe` evaluates indices per frame to accommodate loop or ping-pong playback jumping backwards without keeping state of previous tracks.
2. **Quaternion Normalization**:
   `recalcTransform` normalizes quaternion values on modification to prevent scaling degradation over long run periods.

---

## Context for Resuming Agent

### Important Context
* The C++ engine uses a flat vector structure. Transforms are updated using `transform_utils::setPosition`, `setRotation`, etc., which set flags marking transforms dirty for matrix updates.
* Names in `world.txt` are serialized using scene-relative paths (e.g. `PassengerTruckPath/PathFollow3D/MeshInstance3D`) to act as the primary lookup keys in `nameToIdMapping`.

### Potential Gotchas
* Re-running baking/exporting in Godot requires running **AutoAnimator** first, then **SceneExporter**, to ensure lookups in `world.txt` are properly synchronized.
* Rotation tracks in `world.txt` are serialized as quaternions in `w, x, y, z` ordering, matching the C++ parsing loops.
