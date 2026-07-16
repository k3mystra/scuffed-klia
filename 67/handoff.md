# Handoff: Physics-Based AutoAnimator & Bezier Path Transition

## Table of Contents

- [Session Metadata](#session-metadata)
- [Current State Summary](#current-state-summary)
- [Codebase Understanding](#codebase-understanding)
  - [Architecture Overview](#architecture-overview)
  - [Critical Files](#critical-files)
  - [Key Patterns Discovered](#key-patterns-discovered)
- [Work Completed](#work-completed)
  - [Tasks Finished](#tasks-finished)
  - [Files Modified](#files-modified)
  - [Decisions Made](#decisions-made)
- [Pending Work](#pending-work)
  - [Immediate Next Steps](#immediate-next-steps)
  - [Blockers/Open Questions](#blockersopen-questions)
  - [Deferred Items](#deferred-items)
- [Context for Resuming Agent](#context-for-resuming-agent)
  - [Important Context](#important-context)
  - [Assumptions Made](#assumptions-made)
  - [Potential Gotchas](#potential-gotchas)
- [Environment State](#environment-state)
- [Related Resources](#related-resources)

---

## Session Metadata
- Created: 2026-07-17T01:05:45+08:00
- Project: `c:\Users\tan yu kai\Documents\GitHub\scuffed-klia`
- Branch: `main`
- Session duration: ~2 hours

## Current State Summary

Completed the implementation of physics-based path baking inside `AutoAnimator.gd`. Instead of using hardcoded travel times per segment, the animator now simulates vehicle kinematics (acceleration, top speed, and deceleration) to generate a smooth, high-density keyframe profile (20 FPS). Clamping via a `min_y` metadata check was added, and track target paths are resolved relative to the AnimationPlayer's `root_node`. The current state left off after discussing the potential transition from using separate `Marker3D` nodes to Godot's built-in `Path3D` / `Curve3D` editor workflow.

## Codebase Understanding

### Architecture Overview

`AutoAnimator.gd` is an `@tool` editor script designed to automatically generate 3D translation and rotation keyframes on vehicle nodes. It traverses the scene tree, identifies qualifying nodes (which currently have `Marker3D` children), and writes the baked keyframes into a shared `AnimationPlayer` library clip.

### Critical Files

| File | Purpose | Relevance |
|------|---------|-----------|
| [AutoAnimator.gd](file:///c:/Users/tan%20yu%20kai/Documents/GitHub/scuffed-klia/src/3DScene/LarpCombat/AutoAnimator.gd) | Editor tool script mapping waypoints to animations. | Core script modified and containing the path generation logic. |

### Key Patterns Discovered

* **AnimationPlayer Tracks**: 3D position and rotation tracks in Godot 4 target the Node3D itself, meaning the track path must be relative to the AnimationPlayer's root node without trailing property specifiers (e.g. `Path/To/Vehicle`, not `Path/To/Vehicle:position`).

---

## Work Completed

### Tasks Finished

- [x] **Track NodePath Fix**: Updated track target paths to resolve relatively from the `AnimationPlayer.root_node` so nested vehicles do not default back to the origin when playing.
- [x] **Min Y Clamping**: Removed the bounding-box math and implemented direct clamping to a user-defined `min_y` metadata float on the vehicle.
- [x] **Physics Kinematics**: Added support for vehicle metadata (`acceleration`, `top_speed`) and a marker metadata (`deceleration_point` boolean) to dynamically calculate speed-up and slow-down curves.
- [x] **Dense Keyframe Interpolation**: Implemented high-density keyframe sampling (`SAMPLE_DELTA = 0.05` seconds) using linear interpolation for position and spherical-linear interpolation (slerp) for rotation tangent values.

### Files Modified

| File | Changes | Rationale |
|------|---------|-----------|
| [AutoAnimator.gd](file:///c:/Users/tan%20yu%20kai/Documents/GitHub/scuffed-klia/src/3DScene/LarpCombat/AutoAnimator.gd) | Replaced `bake_path_for_node` implementation. | Integrated relative track path resolution, `min_y` clamping, and the physics kinematic interpolation. |

### Decisions Made

| Decision | Options Considered | Rationale |
|----------|-------------------|-----------|
| Use `deceleration_point` boolean on Marker3D | Vehicle metadata specifying marker name vs. marker boolean metadata | Placing the boolean metadata directly on the marker node is simpler and cleaner for the user to configure in the inspector. |

---

## Pending Work

### Immediate Next Steps

1. **Design/Implement Path3D Integration**: Modify the script to search for a child `Path3D` node (containing a `Curve3D`) rather than discrete `Marker3D` nodes.
2. **Refactor Deceleration Point Logic**: Change deceleration point specification to a percentage/fraction of the path length (e.g. `decel_percent = 80.0` metadata on the vehicle) or a single offset marker since individual waypoint markers won't exist anymore.

### Blockers/Open Questions

- [ ] Question: Confirm if the user prefers to define the deceleration point using a percentage of the path (e.g., `decel_percent = 80.0` metadata) or by checking proximity to a separate marker node when moving to `Path3D`.

### Deferred Items

- None.

---

## Context for Resuming Agent

### Important Context

* The user wants the speed transitions to feel realistic. Straight segments and sudden rotation snaps are currently avoided by doing dense time-grid sampling, but transitioning to a Bezier path (via Godot's `Path3D`) will fully smooth out the physical curves and rotation tangents.
* `Marker3D` nodes were originally chosen to store metadata at each waypoint (which isn't directly supported on individual control points of a `Curve3D` in Godot). Now that travel time is automated using centralized physics, `Path3D` is the preferred workflow going forward.

### Assumptions Made

* It is assumed that the parent node being animated has the metadata fields `acceleration` (float) and `top_speed` (float) defined, falling back to safe defaults of `5.0` and `10.0` respectively if they are absent.

### Potential Gotchas

* When using `Path3D`, Godot's `Curve3D` has built-in caching methods (`sample_baked` and `get_baked_length`). Use these baked queries to easily compute constant-speed arc lengths along the curve instead of writing complex custom numerical Bezier integration.

---

## Environment State

### Tools/Services Used

- Godot Engine 4 editor (tool scripts execute in `@tool` context).
