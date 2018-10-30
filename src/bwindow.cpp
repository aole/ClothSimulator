#include "bwindow.h"

#include <vector>
#include <list>
#include <windowsx.h>
#include <iostream>
#include <tchar.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>

#include <FreeImage.h>

#include "segment.h"

#define MIN_DISTANCE_SQUARE 16
using namespace std;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

BOOST_GEOMETRY_REGISTER_POINT_2D(Vertex, double, cs::cartesian, m_x, m_y);
BOOST_GEOMETRY_REGISTER_LINESTRING(Segment);

std::vector<Segment> segments;

Segment *highlighted_segment = NULL;
Vertex *highlighted_vertex = NULL;

Vertex v1, v2;
std::vector<Shape*> shapes;

double lastx;
double lasty;

bool mousedown;
bool painting;

BLENDFUNCTION blendFn = {0};

HPEN hpen_highlght = CreatePen(PS_SOLID,2,RGB(50,0,205));
HBRUSH hbrush_background = CreateSolidBrush(RGB(200,200,200));

int operation_mode = 0;
FIBITMAP *background_image = NULL;

void BWindow::setMode(int mode)
{
    operation_mode = mode;
}

void BWindow::loadBackground(wchar_t* filename)
{
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

    // check the file signature and deduce its format
    // (the second argument is currently not used by FreeImage)
    fif = FreeImage_GetFileTypeU(filename, 0);
    if(fif == FIF_UNKNOWN)
    {
        // no signature ?
        // try to guess the file format from the file extension
        fif = FreeImage_GetFIFFromFilenameU(filename);
    }
    // check that the plugin has reading capabilities ...
    if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif))
    {
        // ok, let's load the file
        background_image = FreeImage_LoadU(fif, filename, 0);
    }
    InvalidateRect(hwnd, NULL, TRUE);
}

void cleanUp()
{
    for (Shape *s: shapes)
        delete s;
    shapes.clear();
    if (background_image)
        FreeImage_Unload(background_image);
}

void BWindow::reset()
{
    cleanUp();
    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT CALLBACK CanvasProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int x, y;
    BWindow *wndptr = (BWindow*) GetWindowLongPtr(hwnd, GWL_USERDATA);

    switch (message)                  /* handle the messages */
    {
    case WM_NCCREATE:

        blendFn.BlendOp = AC_SRC_OVER;
        blendFn.BlendFlags = 0;
        blendFn.SourceConstantAlpha = 155;
        blendFn.AlphaFormat = 0; //AC_SRC_ALPHA;

        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) ((CREATESTRUCT*)lParam)->lpCreateParams);
        return DefWindowProc (hwnd, message, wParam, lParam);
    case WM_KEYDOWN:
        switch(wParam)
        {
        case VK_DELETE:
            if (highlighted_vertex)
            {
                Shape *s = highlighted_vertex->shape;
                s->removeVertex(highlighted_vertex);
                highlighted_vertex = NULL;
                highlighted_segment = NULL;

                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }
        break;
    case WM_LBUTTONDOWN:
        SetFocus(hwnd);

        mousedown = TRUE;
        lastx = x = GET_X_LPARAM( lParam );
        lasty = y = GET_Y_LPARAM( lParam );
        if (operation_mode==0) // start rectangle
        {
            v1.set(x,y);
            v2.set(x,y);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else if (operation_mode==1) // add vertex
        {
            if(highlighted_segment)
            {
                // check if near any already existing vertex.
                // if so grab it.
                Vertex mouse_point(x, y);
                double min_dist = 10000000;
                Vertex *nearest;

                // TODO: Optimize ... get the first within range
                for (Shape *shape: shapes)
                {
                    for (Vertex *v: shape->m_vertices)
                    {
                        double comp_dist = boost::geometry::comparable_distance(mouse_point, *v);
                        if (comp_dist<min_dist)
                        {
                            min_dist = comp_dist;
                            nearest = v;
                        }
                    }
                }

                if(min_dist<=MIN_DISTANCE_SQUARE)
                {
                    highlighted_vertex = nearest;
                }
                else
                {
                    // if not near any vertex, split the segment
                    // grab the new vertex
                    highlighted_vertex =highlighted_segment->splitAt(x, y);
                    highlighted_segment = NULL;

                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
        }
        SetCapture(hwnd);
        break;
    case WM_LBUTTONUP:
        ReleaseCapture();
        if (mousedown && operation_mode==0)
        {
            x = GET_X_LPARAM( lParam );
            y = GET_Y_LPARAM( lParam );
            v2.set(x,y);
            Shape *shape = new Shape(v1,v2);
            shapes.push_back(shape);

            Segment line1(shape);
            line1.push_back(shape->m_vertices[0]);
            line1.push_back(shape->m_vertices[1]);
            segments.push_back(line1);
            Segment line2(shape);
            line2.push_back(shape->m_vertices[1]);
            line2.push_back(shape->m_vertices[2]);
            segments.push_back(line2);
            Segment line3(shape);
            line3.push_back(shape->m_vertices[2]);
            line3.push_back(shape->m_vertices[3]);
            segments.push_back(line3);
            Segment line4(shape);
            line4.push_back(shape->m_vertices[3]);
            line4.push_back(shape->m_vertices[0]);
            segments.push_back(line4);

            InvalidateRect(hwnd, NULL, TRUE);
        }
        mousedown = FALSE;
        highlighted_vertex = NULL;
        break;
    case WM_MOUSEMOVE:
        x = GET_X_LPARAM( lParam );
        y = GET_Y_LPARAM( lParam );

        // create rectangle
        if (mousedown && operation_mode==0)
        {
            v2.set(x,y);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        // highlight segments
        else if (!mousedown && (operation_mode==1 || operation_mode==2))
        {
            Vertex mouse_point(x, y);

            double min_dist = 10000000;
            Segment *nearest;
            // TODO: Optimize ... get the first within range
            for (Shape *shape: shapes)
            {
                for (Segment *seg: shape->m_segments)
                {
                    double comp_dist = boost::geometry::comparable_distance(mouse_point, *seg);
                    if (comp_dist<min_dist)
                    {
                        min_dist = comp_dist;
                        nearest = seg;
                    }
                };
            }
            if (min_dist<=MIN_DISTANCE_SQUARE)
            {
                highlighted_segment = nearest;
            }
            else
            {
                highlighted_segment = NULL;
            }
            InvalidateRect(hwnd, NULL, TRUE);
        }
        // move segments
        else if (highlighted_segment && mousedown && operation_mode==2)
        {
            highlighted_segment->addPoint(x-lastx,y-lasty);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        // move vertices
        else if (highlighted_vertex && mousedown && operation_mode==1)
        {
            highlighted_vertex->addPoint(x-lastx,y-lasty);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        lastx=x;
        lasty=y;
        break;
    case WM_PAINT:
    {
        if(painting)
        {
            cout<<"painting break"<<endl;
            break;
        }
        painting = TRUE;

        RECT Client_Rect;
        GetClientRect(hwnd,&Client_Rect);
        int win_width = Client_Rect.right - Client_Rect.left;
        int win_height = Client_Rect.bottom + Client_Rect.left;
        HDC memhdc;
        HBITMAP membitmap;
        PAINTSTRUCT ps;

        HDC hdc = BeginPaint(hwnd, &ps);
        memhdc = CreateCompatibleDC(hdc);
        membitmap = CreateCompatibleBitmap(hdc, win_width, win_height);
        SelectObject(memhdc, membitmap);

        FillRect(memhdc,&Client_Rect, hbrush_background);

        // render background image
        if (background_image)
        {
            int w = FreeImage_GetWidth(background_image);
            int h = FreeImage_GetHeight(background_image);

            HDC bithdc = CreateCompatibleDC(hdc);
            HBITMAP bitbitmap = CreateCompatibleBitmap(hdc, w, h);
            SelectObject(bithdc, bitbitmap);
            SetStretchBltMode(bithdc, COLORONCOLOR);
            StretchDIBits(bithdc, 0, 0,
                          w, h,
                          0, 0, w, h,
                          FreeImage_GetBits(background_image), FreeImage_GetInfo(background_image), DIB_RGB_COLORS, SRCCOPY);

            //BitBlt(memhdc, 0, 0, w, h, bithdc, 0, 0, SRCCOPY);

            AlphaBlend(memhdc, 0,0,w,h,bithdc,0,0,w,h,blendFn);
            DeleteObject(bitbitmap);
            DeleteDC(bithdc);
        }
        SelectObject(memhdc, GetStockBrush(NULL_BRUSH));
        SelectObject(memhdc, GetStockObject(BLACK_PEN));

        // all shapes
        for (Shape *shape: shapes)
        {
            for (Segment *seg: shape->m_segments)
            {
                MoveToEx(memhdc, seg->getx(0), seg->gety(0), NULL);
                LineTo(memhdc,seg->getx(1), seg->gety(1));
            };

            if (operation_mode==2 || operation_mode==1)
            {
                for (Vertex *v: shape->m_vertices)
                {
                    Ellipse(memhdc, v->m_x-3, v->m_y-3,v->m_x+3, v->m_y+3);
                }
            }
        }

        // highlighted segment
        if(highlighted_segment && (operation_mode==1 || operation_mode==2))
        {
            SelectObject(memhdc, hpen_highlght);

            double x1 = highlighted_segment->getx(0);
            double y1 = highlighted_segment->gety(0);
            double x2 = highlighted_segment->getx(1);
            double y2 = highlighted_segment->gety(1);

            MoveToEx(memhdc, x1,y1, NULL);
            LineTo(memhdc, x2,y2);
        }

        // temporary rectangle
        if (mousedown && operation_mode==0)
        {
            Rectangle(memhdc, v1.m_x, v1.m_y, v2.m_x, v2.m_y);
        }

        BitBlt(hdc, 0, 0, win_width, win_height, memhdc, 0, 0, SRCCOPY);
        DeleteObject(membitmap);
        DeleteDC    (memhdc);
        DeleteDC    (hdc);
        EndPaint(hwnd, &ps);

        painting = FALSE;
    }
    break;

    case WM_DESTROY:
        cleanUp();

    //case WM_DRAWITEM:
    case WM_ERASEBKGND:
        return (LRESULT)1;// to avoid flicker

    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


HWND BWindow::create(HWND hWndParent, HINSTANCE hInstance)
{
    TCHAR szClassName[ ] = L"Canvas";

    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = CanvasProcedure;      /* This function is called by windows */
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
                                 this                 // object reference
                             );
    ShowWindow (hwnd, SW_SHOWDEFAULT);

    return hwnd;
}
