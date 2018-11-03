#ifndef BWINDOW_H
#define BWINDOW_H

#include <windows.h>
#include <iostream>
#include <vector>

#include "segment.h"
#include "clothchangelistener.h"

class BWindow
{
public:
    HWND create(HWND hWndParent, HINSTANCE hInstance);
    void cleanUp();

    void reset();
    void save(wchar_t *filename);
    void load(wchar_t *filename);

    void addClothChangeListener(ClothChangeListener *listener) { clothChangeListeners.push_back(listener); }

    void setShapeFill(int shape_fill) { this->shape_fill = shape_fill; }
    void setShapeOpacity(int shape_opacity) { this->shape_opacity = shape_opacity; }
    void displayClothes(HDC hdc);

    void setImageOpacity(int image_opacity) { this->image_opacity = image_opacity; }
    void loadImage(wchar_t *filename);
    void clearImage();
    void displayImage(HDC hdc);

    HWND getHandle() { return hwnd; }
    void setSize(int w, int h){ this->window_width=w; this->window_height=h; centerx=w/2; centery=h/2; };
    void drawGrid(HDC hdc);
    void repaint() { InvalidateRect(hwnd, NULL, TRUE); };
    void paint();

    void setMode(int mode);
    int getWidth() { return window_width; }
    int getHeight() { return window_height; }

    void setWidth(int w) { this->window_width=w; centerx=w/2; }
    void setHeight(int g) { this->window_height=g; centery=g/2; }

    void rButtonDown(int x, int y);
    void rButtonUp(int x, int y);
    void lButtonDown(int x, int y);
    void lButtonUp(int x, int y);
    void mouseMove(int x, int y);

    void keyDown(UINT keyCode);

    std::vector<Shape*> shapes;
private:
    vector<ClothChangeListener*> clothChangeListeners;

    void clothChanged()
    {
        for(ClothChangeListener *l: clothChangeListeners)
            l->clothUpdated();
    }

    int shape_fill = 1;
    int shape_opacity = 255;
    int image_opacity = 255;

    Shape *highlighted_shape = NULL;
    Segment *highlighted_segment = NULL;
    Vertex *highlighted_vertex = NULL;

    int last_mouse_x, last_mouse_y;

    int window_width=100,window_height=100;
    int panx=0, pany=0;
    int centerx=50, centery=50;

    HWND hwnd;
    wchar_t image_file[256];

    int gril_gap = 50;
    HPEN hpen_side_grid_lines = CreatePen(PS_SOLID,1,RGB(150,150,150));
    HPEN hpen_main_grid_lines = CreatePen(PS_SOLID,1,RGB(100,100,100));

    bool isRMDown = FALSE;
    bool isLMDown = FALSE;

    bool isPainting = FALSE;
};

#endif // BWINDOW_H
