# 3D DNA Double Helix — CPU-Based Computer Graphics Project

## Overview
A C++17 interactive 3D DNA double-helix visualizer. It uses a CPU software rasterizer: the application constructs triangle geometry, applies 4×4 transformation matrices, performs perspective projection, rasterizes triangles, applies a Z-buffer and basic diffuse lighting. SDL2 is used for window creation, keyboard input and presenting the CPU-generated pixel buffer.

**No OpenGL, Vulkan, Metal, CUDA or GPU rendering pipeline is used.**

## Features
- Parametric double-helix geometry
- Two helical backbones and connecting base-pair rungs
- Four base-pair colors
- 4×4 translation, scaling and X/Y/Z rotation matrices
- Composite model transformation
- Perspective projection
- Triangle rasterization with barycentric coordinates
- Z-buffer depth testing
- Back-face culling and basic diffuse lighting
- Continuous rotation and keyboard controls

## Requirements
- macOS (Intel or Apple Silicon)
- Xcode Command Line Tools
- Homebrew
- CMake
- SDL2

Install dependencies:

```bash
xcode-select --install
brew install cmake sdl2
```

## Build and run

From the project directory:

```bash
cmake -S . -B build
cmake --build build --config Release
./build/dna3d
```

If CMake cannot locate SDL2 on an Apple Silicon Homebrew installation, try:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix)"
cmake --build build --config Release
```

## Controls

| Key | Action |
|---|---|
| Arrow keys | Rotate around X and Y |
| X / Y / Z | Increment rotation around selected axis |
| W / A / S / D | Translate in screen plane |
| + / = | Scale up |
| - | Scale down |
| Space | Pause / resume automatic rotation |
| R | Reset transformations |
| Esc | Exit |

## Graphics pipeline

1. **Model generation:** create the two helical strands and base-pair connectors as triangle meshes.
2. **Model transformation:** multiply each vertex by the composite model matrix.
3. **View/camera transform:** apply the camera offset used by the projection stage.
4. **Perspective projection:** map 3D coordinates to screen coordinates using focal length and depth.
5. **Rasterization:** fill projected triangles using barycentric edge weights.
6. **Depth test:** write a pixel only if its depth is nearer than the stored Z-buffer value.
7. **Shading:** calculate a face normal and simple diffuse intensity.

## DNA geometry

For step index `i`:

- `theta(i) = i × 2π / 10.5`
- `y(i) = (i - (N - 1)/2) × rise`
- Strand A: `(r cos(theta), y, r sin(theta))`
- Strand B: `(-r cos(theta), y, -r sin(theta))`

The strands are joined at each step by a cylinder representing a base-pair rung. This is a stylized educational model, not an atomic-accurate molecular simulation.

## Transformation order

The model matrix is:

`M = T × Rz × Ry × Rx × S`

With column-vector convention, the rightmost operation is applied first: scale, then rotate around X, Y, Z, and finally translate.

## Implementation notes and limitations

- All mesh transformation, projection, triangle filling, depth testing and shading are CPU calculations.
- SDL2 is configured with `SDL_RENDERER_SOFTWARE`; the generated pixel buffer is uploaded to a streaming texture for display.
- This is a compact educational renderer, not a general-purpose graphics engine.
- The current renderer clips triangles when any vertex is behind the near plane instead of implementing full polygon clipping.
- Lighting is flat per triangle. There are no shadows, textures, physically based materials or atomic-level chemical bonds.
- The source is designed for macOS with SDL2's CMake package. It has not been compiled on your particular Mac; build it using the instructions above and report any compiler output for debugging.

## Suggested viva topics
1. What is a double helix?
2. Why are 3D coordinates required?
3. What is homogeneous-coordinate representation?
4. Why use 4×4 matrices for transformations?
5. What is composite transformation?
6. What is perspective projection?
7. What is rasterization?
8. What is a Z-buffer and why is it required?
9. What are barycentric coordinates?
10. What is back-face culling?
11. What is a surface normal?
12. What is the difference between CPU rendering and GPU rendering?
13. Why does transformation order matter?
14. What is the role of SDL2 in this project?

## Project structure

```text
DNA_3D_CPU_Project/
├── src/
│   └── main.cpp
├── include/
├── CMakeLists.txt
└── README.md
```
