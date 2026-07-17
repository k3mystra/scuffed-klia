# Handoff: PathFollow3D Transform Animator Baking (Mesh Target Redirection)

## Table of Contents

- [Session Metadata](#session-metadata)
- [Current State Summary](#current-state-summary)
- [Codebase Understanding](#codebase-understanding)
  - [Architecture Overview](#architecture-overview)
  - [Critical Files](#critical-files)
- [Work Completed](#work-completed)
  - [Tasks Finished](#tasks-finished)
  - [Files Modified](#files-modified)
  - [Decisions Made](#decisions-made)
- [Context for Resuming Agent](#context-for-resuming-agent)
  - [Important Context](#important-context)
  - [Assumptions Made](#assumptions-made)
  - [Potential Gotchas](#potential-gotchas)

---

## Session Metadata
- Created: 2026-07-17T03:55:00+08:00
- Project: `c:\Users\tan yu kai\Documents\GitHub\scuffed-klia`
- Branch: `main`
- Session duration: ~5 hours

## Current State Summary

The `PathFollow3D` progress track baking feature inside `AutoAnimator.gd` has been updated to support child target redirection and verified to be compatible with `SceneExporter.gd`. Instead of writing keyframes directly onto `PathFollow3D`, the script redirects the baked position and rotation tracks to the first child node of `PathFollow3D` (the actual vehicle mesh). It computes the local transforms of the child relative to the starting state of `PathFollow3D` while preserving the design-time local transform offsets.

## Codebase Understanding

### Architecture Overview

The tool script functions as an editor constraint baker. It reads a source animation, samples progress tracks, updates the `PathFollow3D` node, and computes the relative transforms required for the child node to follow the curve when `PathFollow3D` is stationary.

### Critical Files

| File | Purpose | Relevance |
|------|---------|-----------|
| [AutoAnimator.gd](file:///c:/Users/tan%20yu%20kai/Documents/GitHub/scuffed-klia/src/3DScene/LarpCombat/AutoAnimator.gd) | Editor tool script. | Contains the baking and child redirection implementation. |
| [SceneExporter.gd](file:///c:/Users/tan%20yu%20kai/Documents/GitHub/scuffed-klia/src/3DScene/LarpCombat/SceneExporter.gd) | Scene layout and animation exporter. | Exports parsed scene structures and matching animation clips to C++ engine file. |

---

## Work Completed

### Tasks Finished

- [x] **Child Node Redirection**: Automatically detects the first child of `PathFollow3D` (e.g. `Path3D/PathFollow3D/Mesh`) and bakes position and rotation tracks onto it instead of `PathFollow3D`.
- [x] **Transform Space Alignment**: Evaluates child transforms relative to the parent's starting state ($T_{start}^{-1} * T_{follow}(t) * T_{design}$) to preserve local design alignment offsets and keep the mesh positioned accurately when `PathFollow3D` stays at progress `0` at runtime.
- [x] **Reversing with Model Offset**: Flipping (offsetting) the rotation by 180 degrees during the *forward* phase, keeping it unflipped during the *reversing* phase.
- [x] **Smart Re-baking**: Cleans up previous baked tracks of child target nodes.
- [x] **Input & Output Animation separation**: Separates input and output animation processing.
- [x] **SceneExporter Compatibility Verification**: Checked `SceneExporter.gd` parsing loops to ensure that baked child tracks match target mesh entities, and that unhandled progress value/bezier tracks are automatically skipped.

### Files Modified

| File | Changes | Rationale |
|------|---------|-----------|
| [AutoAnimator.gd](file:///c:/Users/tan%20yu%20kai/Documents/GitHub/scuffed-klia/src/3DScene/LarpCombat/AutoAnimator.gd) | Rewrite of sampling logic. | Correctly offset the child's baked transform relative to the starting state of PathFollow3D. |

---

## Context for Resuming Agent

### Important Context

* The vehicle hierarchy is `Path3D` -> `PathFollow3D` -> `Mesh/Vehicle`. The position/rotation tracks are baked directly onto the `Mesh/Vehicle` node.
* At runtime, the `PathFollow3D` node remains stationary at `progress_ratio = 0.0`.
