#include "glwindow.h"

#include <iostream>
#include <cstdio>

#include <SDL2/SDL.h>
#include <gl/glew.h>

SDL_Window* sdlWnd = NULL;
SDL_Window *dummyWnd = NULL;

UINT uiVAOid, uiVBOid;

bool initialized = FALSE;

void display()
{
    float fVert[9];
    fVert[0] = -.5;
    fVert[1] = 0;
    fVert[2] = 0;
    fVert[3] = 1.5;
    fVert[4] = 0;
    fVert[5] = 0;
    fVert[6] = 0;
    fVert[7] = .1;
    fVert[8] = 0;

    glGenVertexArrays(1, &uiVAOid);
    glBindVertexArray(uiVAOid);

    glGenBuffers(1, &uiVBOid);
    glBindBuffer(GL_ARRAY_BUFFER, uiVBOid);

    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(GLfloat), fVert, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glClearColor(.8,.8,.8, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(uiVAOid);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(sdlWnd);
}

LRESULT CALLBACK GLProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_DESTROY:
    {
        //Destroy window
        SDL_DestroyWindow( dummyWnd );
        dummyWnd = NULL;

        SDL_DestroyWindow( sdlWnd );
        sdlWnd = NULL;

        //Quit SDL subsystems
        SDL_Quit();

    }
    return 0;
    case WM_PAINT:
        if(initialized)
            display();
        break;
    case WM_SIZE:
        PostMessage(hwnd, WM_PAINT, 0, 0);
        return 0;

    case WM_LBUTTONDOWN:
        std::cout<<"mouse down"<<std::endl;
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}


HWND GLWindow::create(HWND hWndParent, HINSTANCE hInstance)
{
    TCHAR szClassName[ ] = L"OpenGLWindow";

    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = GLProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS|CS_OWNDC;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    RegisterClassEx (&wincl);

    HWND hwnd = this->hwnd = CreateWindowEx (
                                 WS_EX_CLIENTEDGE,
                                 szClassName,
                                 szClassName,
                                 WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                 x,
                                 y,
                                 w,                 /* The programs width */
                                 h,                 /* and height in pixels */
                                 hWndParent,        /* The window is a child-window */
                                 NULL,
                                 GetModuleHandle(NULL),       /* Program Instance handler */
                                 NULL                 /* No Window Creation data */
                             );

    if (SDL_InitSubSystem(SDL_INIT_VIDEO)<0)
        //if (SDL_Init(SDL_INIT_VIDEO)<0)
        std::cout<<"SDL_InitSubSystem error"<<std::endl;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    dummyWnd = SDL_CreateWindow("a", 0, 0, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
                               );
    if(!dummyWnd)
        std::cout<<"Create dummy Window: "<<SDL_GetError()<<std::endl;

    char sBuf[32];
    std::sprintf(sBuf, "%p", dummyWnd);

    SDL_SetHint(SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, sBuf);

    sdlWnd = SDL_CreateWindowFrom(hwnd);
    if(!sdlWnd)
        std::cout<<"SDL_CreateWindowFrom error"<<std::endl;

    SDL_SetHint(SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, nullptr);

    // below crashes!!!
    //SDL_DestroyWindow(isdlw);

    SDL_GLContext context = SDL_GL_CreateContext(sdlWnd);
    if(!context)
        std::cout<<"Context:"<<SDL_GetError()<<std::endl;

    GLenum err  = glewInit();
    if(err)
        std::cout<<"glewInit error"<<std::endl;

    initialized = TRUE;

    ShowWindow (hwnd, SW_SHOWDEFAULT);

    return hwnd;
}
