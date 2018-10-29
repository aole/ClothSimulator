
#include "openglcontext.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

OpenGLContext::OpenGLContext(void)
{

}

OpenGLContext::OpenGLContext(HWND hwnd)
{
    create30Context(hwnd); // Create a context given a HWND
}

OpenGLContext::~OpenGLContext(void)
{
    wglMakeCurrent(hdc, 0); // Remove the rendering context from our device context
    wglDeleteContext(hrc); // Delete our rendering context

    ReleaseDC(hwnd, hdc); // Release the device context from our window
}

void OnError(int errorCode, const char* msg)
{
    throw std::runtime_error(msg);
}

bool OpenGLContext::create30Context(HWND hwnd)
{
    this->hwnd = hwnd; // Set the HWND for our window
    glfwSetErrorCallback(OnError);
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    //glfwMakeContextCurrent(hwnd);

    return true; // We have successfully created a context, return true
}

void OpenGLContext::reshapeWindow(int w, int h)
{
    windowWidth = w; // Set the window width
    windowHeight = h; // Set the window height
}

void OpenGLContext::renderScene(void)
{
    glViewport(0, 0, windowWidth, windowHeight); // Set the viewport size to fill the window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear required buffers
    SwapBuffers(hdc); // Swap buffers so we can see our rendering
}

