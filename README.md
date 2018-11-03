# ClothSimulator

Program to design to clothes and drape the resulting mesh in 3D.
Design the clothes in 2D window, then simulate the clothes in 3D window to get realistic folds.
The cloth is affected by gravity, collision geometry and other forces.

![Screenshot](screenshots/screenshot08.jpg?raw=true)

Features
--------
Create Rectangle / Move:
Click and Drag LMB on an empty area to create a rectangle in the 2D window.
Click and Drag LMB on a shape to move it.

Create / Move / Delete Vertices:
Click and Drag LMB over an existing vertex to move it.
Click on a segment to create a vertex there (the new vertex will be in drag mode).
Click and Hold LMB over the vertex and click Delete key to dissolve the vertex.

Move Segments:
Click and Drag LMB over a line segment to move it.

Pan / Zoom 3D Window:
Click and Drag RMB.
Rotate mouse wheel to zoom.

Pan 2D Window:
Click and Drag RMB.

Open an image file to display it over the 2D window.

Save and Load 2D Shapes (with background) to a text file.

Preferences file: set background image opacity. set shape fill/no fill.

How to Compile
--------------
Download SDL2: https://www.libsdl.org/download-2.0.php
Point to include and lib in your project.
Copy SDL2.dll in the project folder

Download GLEW (binaries): http://glew.sourceforge.net/
Point to include and lib in your project.

Download FreeImage(binaries): http://freeimage.sourceforge.net/download.html
Point to include and lib in your project.

Download GLM header files: https://github.com/g-truc/glm/tags
Point to include in your project

Troubleshooting
---------------
UNICODE

GLEW_STATIC

WINVER>=0x0501 (for use of AlphaBlend)
