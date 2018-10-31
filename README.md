# ClothSimulator

![Screenshot](screenshots/screenshot04.jpg?raw=true)

Features
--------
Create Rectangle / Move:
Click and Drag LMB on an empty area to create a rectangle in the 2D window.
Click and Drag LMB on a shape to move it.

Create / Move Vertices:
Click and Drag LMB over an existing vertex to move it.
Click on a segment to create a vertex there (the new vertex will be in drag mode).

Move Segments:
Click and Drag LMB over a line segment to move it.

Open an image file to display it over the 2D window.

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

Troubleshooting
---------------
UNICODE

GLEW_STATIC

WINVER>=0x0501 (for use of AlphaBlend)
