# CPU-3D-Renderer

TinyRenderer is a software-based 3D rendering engine.
It does not use OpenGL or DirectX. Instead, it implements the 3D graphics pipeline completely in C++, allowing you to understand how rendering works internally.

---

## Features
- Load and render **.obj 3D models**
- Software rasterization of triangles
- Simulated **vertex and fragment shaders** in pure C++
- Perspective projection and camera transformations
- Z-buffer for depth handling
- Diffuse, specular, and normal mapping
- Outputs the final rendered image as a **TGA file** (CPU-only)

---
## How to Build

1. Compile the project using `g++` (requires C++17 or later):
   ```bash
   g++ -std=c++17 main.cpp model.cpp our_gl.cpp tgaimage.cpp -o renderer.exe
   ./renderer obj/<.obj filename> 
2. After rendering an output file will be created with the below name:
    framebuffer.tga
