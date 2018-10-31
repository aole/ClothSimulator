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

    void setShapeFill(int shape_fill) { this->shape_fill = shape_fill; }
    void setShapeOpacity(int shape_opacity) { this->shape_opacity = shape_opacity; }
    void loadImage(wchar_t *filename, int background_opacity);
    void clearImage();

    int shape_fill = 1;
    int shape_opacity = 255;
    int background_opacity = 255;

protected:
private:
    int x=0,y=0,w=100,h=100;
    HWND hwnd;
};

#endif // BWINDOW_H
