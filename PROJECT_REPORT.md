# Project Report: 3D DNA Double Helix Using Computer Graphics

## 1. Title
3D DNA Double Helix Visualization with Geometric Transformations using C++.

## 2. Objective
To create an interactive three-dimensional representation of a DNA double helix and demonstrate fundamental computer-graphics operations, including translation, rotation, scaling, perspective projection, rasterization and hidden-surface removal.

## 3. Software and tools
- C++17
- SDL2 for window management and input
- CMake build system
- CPU software rasterizer

## 4. Methodology
The DNA strands are generated parametrically using trigonometric functions. Each strand follows a helical path around a central axis. Cylindrical meshes connect successive backbone points and corresponding points across the two strands. The geometry is represented as triangles.

A model matrix combines scaling, rotations and translation. The transformed vertices are projected onto a two-dimensional viewport. The rasterizer fills each projected triangle and uses a Z-buffer to determine which surface is visible at each pixel. A face-normal calculation provides basic diffuse lighting.

## 5. Transformation equations
Translation:
`T = [[1,0,0,tx],[0,1,0,ty],[0,0,1,tz],[0,0,0,1]]`

Scaling:
`S = diag(sx, sy, sz, 1)`

Rotation about Z:
`Rz = [[cosθ,-sinθ,0,0],[sinθ,cosθ,0,0],[0,0,1,0],[0,0,0,1]]`

Composite model transformation:
`M = T × Rz × Ry × Rx × S`

## 6. Result
The program displays an interactive 3D DNA model. The user can rotate, translate and scale the model, pause or resume automatic rotation, and reset the transformation state.

## 7. Conclusion
The project demonstrates how a three-dimensional object can be generated and displayed through a CPU-based graphics pipeline. It applies mathematical transformations and fundamental rendering techniques without relying on a GPU graphics API.

## 8. Future scope
- Full near-plane clipping
- Smooth vertex lighting
- Mouse-based camera control
- Labels for A, T, G and C
- Export of rendered frames
- More detailed molecular geometry
