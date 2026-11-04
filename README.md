

# ClothSimulator

Program to design to clothes and drape the resulting mesh in 3D.

Design the clothes in 2D window, then simulate the clothes in 3D window to get realistic folds.

The cloth is affected by gravity, collision geometry and other forces.

![Screenshot](screenshots/demo01.gif?raw=true)
![Screenshot](screenshots/demo02.gif?raw=true)
![Screenshot](screenshots/screenshot01.jpg?raw=true)

How to Use
----------
* Click and Drag LMB on an empty area to create a rectangle in the 2D window.
* Click and Drag LMB on a shape to move it.
* Click and Drag LMB over an existing vertex to move it.
* Hold Ctrl and LMB Click on a segment to create a vertex there (the new vertex will be in drag mode).
* Click and Drag MMB to Pan 2D Window.
* Drag and drop an image file to display it over the 2D window.
* Simulate Cloth physics (press SPACEBAR to toggle simulation)

Compile Dependency
------------------
- wxWidgets
- glew
- glm

Build and Run on Windows
------------------------
Install Visual Studio 2022/2026 or its Build Tools with **Desktop development with
C++**, a Windows SDK, and **C++ CMake tools for Windows**. Git must be on PATH.

From PowerShell in this folder:

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
powershell -ExecutionPolicy Bypass -File .\run.ps1
```

The first build downloads vcpkg and builds the pinned wxWidgets, GLEW, and GLM
dependencies. Later builds reuse them. The default is a 64-bit Release build;
pass `-Configuration Debug` to both scripts for a Debug build.

The executable is `build\Release\ClothSimulator.exe`. Use `run.ps1` to start it
with the correct working directory for shaders and sample objects. OpenGL 3.2
driver support is required. The generated `build\ClothSimulator.sln` can also be
opened in Visual Studio; use this instead of the legacy root solution, which
references an older toolset and hard-coded SDK paths.
