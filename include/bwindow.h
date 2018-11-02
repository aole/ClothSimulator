#ifndef BWINDOW_H
#define BWINDOW_H

#include <windows.h>
#include <iostream>

class BWindow
{
public:
    HWND create(HWND hWndParent, HINSTANCE hInstance);

    void setMode(int mode);

    void setSize(int w, int h){ this->w=w; this->h=h; };

    HWND getHandle() { return hwnd; }

    void reset();

    void save(wchar_t *filename);
    void load(wchar_t *filename);

    void setShapeFill(int shape_fill) { this->shape_fill = shape_fill; }
    void setShapeOpacity(int shape_opacity) { this->shape_opacity = shape_opacity; }
    void setImageOpacity(int image_opacity) { this->image_opacity = image_opacity; }
    void loadImage(wchar_t *filename);
    void clearImage();

    int shape_fill = 1;
    int shape_opacity = 255;
    int image_opacity = 255;

    void drawGrid(HDC hdc);
    void repaint() { InvalidateRect(hwnd, NULL, TRUE); };

    int getWidth() { return w; }
    int getHeight() { return h; }

    void setWidth(int w) { this->w=w; }
    void setHeight(int h) { this->h=h; }

    void rButtonDown(int x, int y);
    void rButtonUp(int x, int y);
    void mouseMove(int x, int y);

protected:
private:
    int w=100,h=100;
    int panx=0, pany=0;
    HWND hwnd;
    wchar_t image_file[256];

    int gril_gap = 50;
    HPEN hpen_side_grid_lines = CreatePen(PS_SOLID,1,RGB(150,150,150));
    HPEN hpen_main_grid_lines = CreatePen(PS_SOLID,1,RGB(100,100,100));

    bool isRBDown = FALSE;
    int last_mouse_x, last_mouse_y;
};

#endif // BWINDOW_H
