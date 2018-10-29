#ifndef OPENGLCONTEXT_H
#define OPENGLCONTEXT_H

#include <windows.h>

class OpenGLContext
{
public:
    OpenGLContext(void); // Default constructor
    OpenGLContext(HWND hwnd); // Constructor for creating our context given a hwnd
    ~OpenGLContext(void); // Destructor for cleaning up our application
    bool create30Context(HWND hwnd); // Creation of our OpenGL 3.x context
    void setupScene(void); // All scene information can be setup here
    void reshapeWindow(int w, int h); // Method to get our window width and height on resize
    void renderScene(void); // Render scene (display method from previous OpenGL tutorials)

protected:
    HGLRC hrc; // Rendering context
    HDC hdc; // Device context
    HWND hwnd; // Window identifier
private:
    int windowWidth; // Store the width of our window
    int windowHeight; // Store the height of our window
};

#endif // OPENGLCONTEXT_H
