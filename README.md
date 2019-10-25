# ClothSimulator

Program to design to clothes and drape the resulting mesh in 3D.
Design the clothes in 2D window, then simulate the clothes in 3D window to get realistic folds.
The cloth is affected by gravity, collision geometry and other forces.

![Screenshot](screenshots/demo01.gif?raw=true)
![Screenshot](screenshots/demo02.gif?raw=true)

Features
--------
Create / Move / Delete Rectangle:
Click and Drag LMB on an empty area to create a rectangle in the 2D window.
Click and Drag LMB on a shape to move it.
Click and Hold LMB over the shape and click Delete key to delete it.

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

Compile Dependency
------------------
- wxWidgets
- glew
- glm
