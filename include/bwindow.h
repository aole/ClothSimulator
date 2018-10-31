#ifndef BWINDOW_H
#define BWINDOW_H

#include <windows.h>
#include <iostream>

class BWindow
{
public:
    HWND create(HWND hWndParent, HINSTANCE hInstance);

    void setMode(int mode);

    void setSize(int x, int y, int w, int h){ this->x=x; this->y=y; this->w=w; this->h=h; };

    HWND getHandle() { return hwnd; }

    void reset();

    void setShapeOpacity(int shape_opacity) { this->shape_opacity = shape_opacity; }
    void loadBackground(wchar_t *filename, int background_opacity);
    void clearBackground();

    int shape_opacity = 255;
    int background_opacity = 255;

protected:
private:
    int x=0,y=0,w=100,h=100;
    HWND hwnd;
};

#endif // BWINDOW_H
