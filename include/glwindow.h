#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <windows.h>

class GLWindow
{
public:
    HWND create(HWND hWndParent, HINSTANCE hInstance);

    void setSize(int x, int y, int w, int h){ this->x=x; this->y=y; this->w=w; this->h=h; };

    HWND getHandle() { return hwnd; }
protected:

private:
    int x=0,y=0,w=100,h=100;
    HWND hwnd;
};

#endif // GLWINDOW_H
