#ifndef BWINDOW_H
#define BWINDOW_H

#include <windows.h>
#include <iostream>
#include <vector>

#include "segment.h"

class BWindow
{
public:
    HWND create(HWND hWndParent, HINSTANCE hInstance);
    void cleanUp();

    void reset();
    void save(wchar_t *filename);
    void load(wchar_t *filename);

    void setShapeFill(int shape_fill) { this->shape_fill = shape_fill; }
    void setShapeOpacity(int shape_opacity) { this->shape_opacity = shape_opacity; }
    void setImageOpacity(int image_opacity) { this->image_opacity = image_opacity; }
    void loadImage(wchar_t *filename);
    void clearImage();

    HWND getHandle() { return hwnd; }
    void setSize(int w, int h){ this->w=w; this->h=h; };
    void drawGrid(HDC hdc);
    void repaint() { InvalidateRect(hwnd, NULL, TRUE); };
    void paint();

    void setMode(int mode);
    int getWidth() { return w; }
    int getHeight() { return h; }

    void setWidth(int w) { this->w=w; }
    void setHeight(int h) { this->h=h; }

    void rButtonDown(int x, int y);
    void rButtonUp(int x, int y);
    void lButtonDown(int x, int y);
    void lButtonUp(int x, int y);
    void mouseMove(int x, int y);

    void keyDown(UINT keyCode);

    void clothChangedAck() { clothChanged = FALSE; }
private:
    int shape_fill = 1;
    int shape_opacity = 255;
    int image_opacity = 255;

    std::vector<Shape*> shapes;

    Shape *highlighted_shape = NULL;
    Segment *highlighted_segment = NULL;
    Vertex *highlighted_vertex = NULL;

    int last_mouse_x, last_mouse_y;

    int w=100,h=100;
    int panx=0, pany=0;
    HWND hwnd;
    wchar_t image_file[256];

    int gril_gap = 50;
    HPEN hpen_side_grid_lines = CreatePen(PS_SOLID,1,RGB(150,150,150));
    HPEN hpen_main_grid_lines = CreatePen(PS_SOLID,1,RGB(100,100,100));

    bool isRMDown = FALSE;
    bool isLMDown = FALSE;

    bool isPainting = FALSE;

    bool clothChanged = FALSE;
};

#endif // BWINDOW_H
