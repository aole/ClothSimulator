#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "btoolbar.h"
#include "bwindow.h"
#include "glwindow.h"

#define IDC_MAIN_TOOL   10000
#define ID_CREATE_RECT  10001
#define ID_ADD_VERTEX   10002
#define ID_MOVE_SEGMENT 10003

#define ID_FILE         20000
#define ID_FILE_EXIT    20001
#define ID_FILE_RESET   20002
#define ID_FILE_BACKIMG 20003

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CanvasProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("Cloth Simulator");

HINSTANCE hInstance;
HWND hwndMain;

using namespace std;

int window_width = 800;
int window_height = 600;

void readPreferencesFile()
{
    ifstream pref_file("pref.txt");
    string line;
    while(getline(pref_file,line))
    {
        int pos = line.find("=");
        if(pos<1)
            continue;
        string key = line.substr(0,pos);
        boost::trim(key);
        string value = line.substr(pos+1,-1);
        boost::trim(value);

        if (key=="width")
            window_width = boost::lexical_cast<int>(value);
        else if (key=="height")
            window_height = boost::lexical_cast<int>(value);
    }
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    // get preferences
    readPreferencesFile() ;

    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
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
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = hwndMain = CreateWindowEx (
                          0,                   /* Extended possibilites for variation */
                          szClassName,         /* Classname */
                          szClassName,       /* Title Text */
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, /* default window */
                          CW_USEDEFAULT,       /* Windows decides the position */
                          CW_USEDEFAULT,       /* where the window ends up on the screen */
                          window_width,                 /* The programs width */
                          window_height,                 /* and height in pixels */
                          HWND_DESKTOP,        /* The window is a child-window to desktop */
                          NULL,                /* No menu */
                          hThisInstance,       /* Program Instance handler */
                          NULL                 /* No Window Creation data */
                      );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

BToolBar toolbar;
BWindow canvas;
GLWindow glwindow;

void CreateUI(HWND hWndParent)
{
    // menubar
    HMENU hMenu, hMenuFile;

    hMenu  = CreateMenu();
    hMenuFile = CreatePopupMenu();
    AppendMenu(hMenu, MF_POPUP, (UINT)hMenuFile, L"&File");

    AppendMenu(hMenuFile, MF_STRING, ID_FILE_RESET, L"&Reset");
    AppendMenu(hMenuFile, MF_STRING, ID_FILE_BACKIMG, L"Background &Image...");
    AppendMenu(hMenuFile, MF_STRING, ID_FILE_EXIT, L"E&xit");

    SetMenu(hWndParent, hMenu);

    // toolbar
    toolbar.addCheckGroup(STD_FILENEW, ID_CREATE_RECT, TRUE);
    toolbar.addCheckGroup(STD_DELETE, ID_ADD_VERTEX, FALSE);
    toolbar.addCheckGroup(STD_COPY, ID_MOVE_SEGMENT, FALSE);
    toolbar.create(hWndParent);

    RECT wrect;
    GetClientRect(hWndParent, &wrect);

    RECT tbrect;
    GetWindowRect(toolbar.getHandle(), &tbrect);
    // canvas
    canvas.setSize(wrect.left+(wrect.right-wrect.left)/2+1,
                   wrect.top+tbrect.bottom-tbrect.top,
                   wrect.right-wrect.left,
                   wrect.bottom-wrect.top-(tbrect.bottom-tbrect.top));
    canvas.create(hWndParent, hInstance);

    // opengl window
    glwindow.setSize(wrect.left,
                     wrect.top+tbrect.bottom-tbrect.top,
                     (wrect.right-wrect.left)/2-1,
                     wrect.bottom-wrect.top-(tbrect.bottom-tbrect.top));
    glwindow.create(hWndParent, hInstance);

}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
        CreateUI(hwnd);
        break;
    case WM_SIZE:
        toolbar.adjustSize();
        RECT wrect;
        GetClientRect(hwnd, &wrect);

        RECT tbrect;
        GetWindowRect(toolbar.getHandle(), &tbrect);

        MoveWindow(canvas.getHandle(),wrect.left+(wrect.right-wrect.left)/2+1,
                   wrect.top+tbrect.bottom-tbrect.top,
                   wrect.right-wrect.left,
                   wrect.bottom-wrect.top-(tbrect.bottom-tbrect.top),TRUE);

        MoveWindow(glwindow.getHandle(),wrect.left,
                   wrect.top+tbrect.bottom-tbrect.top,
                   (wrect.right-wrect.left)/2-1,
                   wrect.bottom-wrect.top-(tbrect.bottom-tbrect.top),TRUE);

        return DefWindowProc (hwnd, message, wParam, lParam);
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_CREATE_RECT:
            canvas.setMode(0);
            break;
        case ID_ADD_VERTEX:
            canvas.setMode(1);
            break;
        case ID_MOVE_SEGMENT:
            canvas.setMode(2);
            break;
        case ID_FILE_RESET:
            canvas.reset();
            break;
        case ID_FILE_EXIT:
            PostQuitMessage(0);
            break;
        case ID_FILE_BACKIMG:
        {
            wchar_t filename[256];

            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof (OPENFILENAME);
            ofn.hwndOwner = hwnd;
            ofn.nMaxFile = sizeof(filename);
            ofn.lpstrFile = filename;
            ofn.lpstrFile[0] = '\0';
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

            if (GetOpenFileName(&ofn)==TRUE){
                canvas.loadBackground(ofn.lpstrFile);
            }
        }
        break;
        }
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
