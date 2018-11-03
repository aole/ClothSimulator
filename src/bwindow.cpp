#include "bwindow.h"

#include <list>
#include <windowsx.h>
#include <iostream>
#include <tchar.h>
#include <fstream>
#include <string>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/foreach.hpp>

#include <FreeImage.h>

#define MIN_DISTANCE_SQUARE 16

using namespace std;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

BOOST_GEOMETRY_REGISTER_POINT_2D(Vertex, double, cs::cartesian, m_x, m_y);
BOOST_GEOMETRY_REGISTER_LINESTRING(Segment);

Vertex v1, v2;

BLENDFUNCTION blendFn = {0};

HPEN hpen_highlght = CreatePen(PS_SOLID,2,RGB(50,0,205));
HPEN hpen_shape_grid = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));

HBRUSH hbrush_background = CreateSolidBrush(RGB(200,200,200));
HBRUSH hbrush_fill = CreateSolidBrush(RGB(255,255,255));

int operation_mode = 0;

FIBITMAP *background_image = NULL;
int bg_width = 0;
int bg_height = 0;

void BWindow::cleanUp()
{
    for (Shape *s: shapes)
        delete s;
    shapes.clear();

    if (background_image)
    {
        FreeImage_Unload(background_image);
        background_image = NULL;
    }

    clothChanged = TRUE;
}

void BWindow::save(wchar_t* filename)
{
    std::wstring wsf(filename);
    std::string sf(wsf.begin(), wsf.end());
    std::wofstream outfile;
    outfile.open(sf.c_str());
    if(!outfile.is_open())
    {
        std::wcerr << "Couldn't open "<< filename << std::endl;
        return;
    }

    outfile << L"image=" << std::wstring(image_file) << std::endl;
    for(Shape *s: shapes)
    {
        outfile << L"shape=" << s->getName() << std::endl;
        s->save(outfile);
    }
    outfile.close();
}

void BWindow::load(wchar_t* filename)
{
    std::wstring wsf(filename);
    std::string sf(wsf.begin(), wsf.end());
    std::wifstream outfile;
    outfile.open(sf.c_str());
    if(!outfile.is_open())
    {
        std::wcerr << "Couldn't open "<< filename << std::endl;
        return;
    }

    cleanUp();

    std::wstring line;
    Shape *shape = NULL;
    while(std::getline(outfile,line))
    {

        int pos = line.find(L"=");
        if(pos<1)
            continue;
        wstring key = line.substr(0,pos);
        boost::trim(key);
        wstring value = line.substr(pos+1,-1);
        boost::trim(value);
        if (key==L"shape")
        {
            shape = new Shape();
            shapes.push_back(shape);
        }
        else if (key==L"image")
        {
            if(value.size())
            {
                wcsncpy(image_file, value.c_str(), 256);
                loadImage(image_file);
            }
        }
        else
        {
            if(shape)
            {
                shape->process(key, value);
            }
        }
    }

    clothChanged = TRUE;
}

void BWindow::setMode(int mode)
{
    operation_mode = mode;
    InvalidateRect(hwnd, NULL, TRUE);
}

void BWindow::loadImage(wchar_t* filename)
{
    wcsncpy(image_file, filename, 256);

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
        blendFn.BlendOp = AC_SRC_OVER;
        blendFn.BlendFlags = 0;
        blendFn.SourceConstantAlpha = image_opacity;
        blendFn.AlphaFormat = 0; //AC_SRC_ALPHA;

        background_image = FreeImage_LoadU(fif, filename, 0);
        bg_width = FreeImage_GetWidth(background_image);
        bg_height = FreeImage_GetHeight(background_image);
    }
    InvalidateRect(hwnd, NULL, TRUE);
}

void BWindow::clearImage()
{
    if (background_image)
    {
        FreeImage_Unload(background_image);
        background_image = NULL;
    }
    InvalidateRect(hwnd, NULL, TRUE);
}

void BWindow::reset()
{
    for (Shape *s: shapes)
        delete s;
    shapes.clear();

    InvalidateRect(hwnd, NULL, TRUE);
    clothChanged = TRUE;
}

void BWindow::rButtonDown(int x, int y)
{
    isRMDown = TRUE;
    last_mouse_x = x;
    last_mouse_y = y;
}

void BWindow::rButtonUp(int x, int y)
{
    isRMDown = FALSE;
    last_mouse_x = x;
    last_mouse_y = y;
}

void BWindow::lButtonDown(int x, int y)
{
    last_mouse_x = x;
    last_mouse_y = y;
    isLMDown = TRUE;

    if (operation_mode==0) // start rectangle
    {
        Vertex mouse_point(x-(centerx+panx), y-(centery+pany));
        // check if inside a shape, then start move
        highlighted_shape = NULL;
        BOOST_REVERSE_FOREACH(Shape *shape, shapes)
        {
            if(shape->within(&mouse_point))
            {
                highlighted_shape = shape;
                break;
            }
        }
        if (!highlighted_shape)
        {
            // start creating rectangle shape
            v1.set(x, y);
            v2.set(x, y);
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
    else if (operation_mode==1) // add vertex
    {
        if(highlighted_segment)
        {
            // check if near any already existing vertex.
            // if so grab it.
            Vertex mouse_point(x-(centerx+panx), y-(centery+pany));
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
                highlighted_vertex = highlighted_segment->splitAt(x-(centerx+panx), y-(centery+pany));
                highlighted_segment = NULL;

                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
    }
    SetCapture(hwnd);
}

void BWindow::lButtonUp(int x, int y)
{
    if (isLMDown && operation_mode==0 && !highlighted_shape)
    {
        v1.m_x -= (centerx+panx);
        v1.m_y -= (centery+pany);
        v2.set(x-(centerx+panx),y-(centery+pany));

        Shape *shape = new Shape(v1,v2);
        shapes.push_back(shape);
        InvalidateRect(hwnd, NULL, TRUE);
        clothChanged = TRUE;
    }
    isLMDown = FALSE;
    highlighted_vertex = NULL;
    InvalidateRect(hwnd, NULL, TRUE);
}

void BWindow::mouseMove(int x, int y)
{
    int dx = x - last_mouse_x;
    int dy = y - last_mouse_y;

    if(isRMDown)
    {

        panx += dx;
        pany += dy;

        InvalidateRect(hwnd, NULL, TRUE);
        //repaint();
    }

    else if(isLMDown)
    {
        // create rectangle
        if (isLMDown && operation_mode==0)
        {
            if(highlighted_shape)
                highlighted_shape->translate(dx, dy);
            else
                v2.set(x, y);

            InvalidateRect(hwnd, NULL, TRUE);
        }
        // move segments
        else if (highlighted_segment && isLMDown && operation_mode==2)
        {
            highlighted_segment->addPoint(dx, dy);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        // move vertices
        else if (highlighted_vertex && isLMDown && operation_mode==1)
        {
            highlighted_vertex->addPoint(dx, dy);
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }

    // highlight segments
    else  if ((operation_mode==1 || operation_mode==2))
    {
        Vertex mouse_point(x-(centerx+panx), y-(centery+pany));

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

    last_mouse_x = x;
    last_mouse_y = y;
}

void BWindow::drawGrid(HDC hdc)
{
    SelectObject(hdc, hpen_side_grid_lines);

    int cx = panx+centerx;
    int cy = pany+centery;

    // horizontal
    for(int y=(cy>0?cy:0) - gril_gap; y>0; y-=gril_gap)
    {
        MoveToEx(hdc, 0, y, NULL);
        LineTo(hdc, window_width, y);
    }
    for(int y=(cy<window_height?cy:window_height) + gril_gap; y<window_height; y+=gril_gap)
    {
        MoveToEx(hdc, 0, y, NULL);
        LineTo(hdc, window_width, y);
    }
// vertical
    for(int x=(cx>0?cx:0) - gril_gap; x>0; x-=gril_gap)
    {
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, window_height);
    }
    for(int x=(cx<window_width?cx:window_width) + gril_gap; x<window_width; x+=gril_gap)
    {
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, window_height);
    }

    SelectObject(hdc, hpen_main_grid_lines);

// horizontal main
    if(cy>0 && cy<window_width)
    {
        MoveToEx(hdc, 0, cy, NULL);
        LineTo(hdc, window_width, cy);
    }
// vertical main
    if(cx>0 && cx<window_height)
    {
        MoveToEx(hdc, cx, 0, NULL);
        LineTo(hdc, cx, window_height);
    }
}

void BWindow::keyDown(UINT keyCode)
{
    switch(keyCode)
    {
    case VK_DELETE:
        if (highlighted_vertex)
        {
            Shape *s = highlighted_vertex->shape;
            s->removeVertex(highlighted_vertex);
            highlighted_vertex = NULL;
            highlighted_segment = NULL;

            InvalidateRect(hwnd, NULL, TRUE);
            clothChanged = TRUE;
        }
        break;
    }
}

void BWindow::displayImage(HDC hdc)
{
    if (background_image)
    {
        HDC bithdc = CreateCompatibleDC(hdc);
        HBITMAP bitbitmap = CreateCompatibleBitmap(hdc, bg_width, bg_height);
        SelectObject(bithdc, bitbitmap);
        SetStretchBltMode(bithdc, COLORONCOLOR);
        StretchDIBits(bithdc, 0, 0,
                      bg_width, bg_height,
                      0, 0, bg_width, bg_height,
                      FreeImage_GetBits(background_image), FreeImage_GetInfo(background_image), DIB_RGB_COLORS, SRCCOPY);

        AlphaBlend(hdc, panx+centerx-bg_width/2, pany+centery-bg_height/2, bg_width,bg_height,bithdc,0,0,bg_width,bg_height,blendFn);
        DeleteObject(bitbitmap);
        DeleteDC(bithdc);
    }
}

void BWindow::displayClothes(HDC hdc)
{
    SelectObject(hdc, hbrush_fill);
    SelectObject(hdc, GetStockObject(BLACK_PEN));

    // all shapes
    for (Shape *shape: shapes)
    {
        if(!isLMDown)
        {
            // render shape grid
            SelectObject(hdc, hpen_shape_grid);
            //shape->RenderGrid(hdc);
        }

        // render shape outline
        SelectObject(hdc, GetStockObject(BLACK_PEN));

        int num_points = shape->m_segments.size()+1;
        POINT points[num_points];
        int i=0;
        for (Segment *seg: shape->m_segments)
        {
            if(shape_fill)
            {
                if(!i)
                    points[i++] = {(int)seg->getx(0)+centerx+panx, (int)seg->gety(0)+centery+pany};
                points[i++] = {(int)seg->getx(1)+centerx+panx, (int)seg->gety(1)+centery+pany};
            }
            else
            {
                MoveToEx(hdc, (int)seg->getx(0)+centerx+panx, (int)seg->gety(0)+centery+pany, NULL);
                LineTo(hdc, (int)seg->getx(1)+centerx+panx, (int)seg->gety(1)+centery+pany);
            }
        };

        if(shape_fill)
            Polygon(hdc, points, num_points);

        if (operation_mode==2 || operation_mode==1)
        {
            for (Vertex *v: shape->m_vertices)
            {
                Ellipse(hdc, (int)v->m_x-3+centerx+panx, (int)v->m_y-3+centery+pany,
                        (int)v->m_x+3+centerx+panx, (int)v->m_y+3+centery+pany);
            }
        }

    }
}

void BWindow::paint()
{
    if(isPainting)
    {
        cout<<"painting break"<<endl;
        return;
    }
    isPainting = TRUE;

    RECT Client_Rect;
    GetClientRect(hwnd,&Client_Rect);
    int win_width = window_width; //Client_Rect.right - Client_Rect.left;
    int win_height = window_height; //Client_Rect.bottom + Client_Rect.left;
    HDC memhdc;
    HBITMAP membitmap;
    PAINTSTRUCT ps;

    HDC hdc = BeginPaint(hwnd, &ps);
    memhdc = CreateCompatibleDC(hdc);
    membitmap = CreateCompatibleBitmap(hdc, win_width, win_height);
    SelectObject(memhdc, membitmap);

    FillRect(memhdc,&Client_Rect, hbrush_background);

    drawGrid(memhdc);

    displayClothes(memhdc);

    // highlighted vertex
    if(highlighted_vertex && (operation_mode==1))
    {
        SelectObject(memhdc, hpen_highlght);

        Ellipse(memhdc, (int)highlighted_vertex->m_x-3+centerx+panx, (int)highlighted_vertex->m_y-3+centery+pany,
                (int)highlighted_vertex->m_x+3+centerx+panx, (int)highlighted_vertex->m_y+3+centery+pany);
    }
    // highlighted segment
    else if(highlighted_segment && (operation_mode==1 || operation_mode==2))
    {
        SelectObject(memhdc, hpen_highlght);

        double x1 = highlighted_segment->getx(0);
        double y1 = highlighted_segment->gety(0);
        double x2 = highlighted_segment->getx(1);
        double y2 = highlighted_segment->gety(1);

        MoveToEx(memhdc, x1+centerx+panx,y1+centery+pany, NULL);
        LineTo(memhdc, x2+centerx+panx,y2+centery+pany);
    }

    // temporary (while creating) rectangle
    if (isLMDown && operation_mode==0 && !highlighted_shape)
    {
        if(shape_fill)
            Rectangle(memhdc, (int)v1.m_x, (int)v1.m_y, (int)v2.m_x, (int)v2.m_y);
        else
        {
            MoveToEx(memhdc, (int)v1.m_x, (int)v1.m_y, NULL);
            LineTo(memhdc, (int)v2.m_x, (int)v1.m_y);
            LineTo(memhdc, (int)v2.m_x, (int)v2.m_y);
            LineTo(memhdc, (int)v1.m_x, (int)v2.m_y);
            LineTo(memhdc, (int)v1.m_x, (int)v1.m_y);
        }
    }

    displayImage(memhdc);

    BitBlt(hdc, 0, 0, win_width, win_height, memhdc, 0, 0, SRCCOPY);
    DeleteObject(membitmap);
    DeleteDC    (memhdc);
    DeleteDC    (hdc);
    EndPaint(hwnd, &ps);

    isPainting = FALSE;
}

LRESULT CALLBACK CanvasProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    BWindow *wndptr = (BWindow*) GetWindowLongPtr(hwnd, GWL_USERDATA);

    switch (message)                  /* handle the messages */
    {
    case WM_NCCREATE:

        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) ((CREATESTRUCT*)lParam)->lpCreateParams);
        return DefWindowProc (hwnd, message, wParam, lParam);

    case WM_MOVE:
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        wndptr->setWidth(rect.right - (int)(short) LOWORD(lParam));
    }
    break;
    case WM_SIZE:
        wndptr->setHeight(HIWORD(lParam));
        break;

    case WM_KEYDOWN:
        wndptr->keyDown(wParam);
        break;

    case WM_RBUTTONDOWN:
        SetCapture(hwnd);
        wndptr->rButtonDown(GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ));
        break;
    case WM_RBUTTONUP:
        ReleaseCapture();
        wndptr->rButtonUp(GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ));
        break;
    case WM_LBUTTONDOWN:
        SetFocus(hwnd);
        SetCapture(hwnd);
        wndptr->lButtonDown(GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ));
        break;
    case WM_LBUTTONUP:
        ReleaseCapture();
        wndptr->lButtonUp(GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ));
        break;
    case WM_MOUSEMOVE:
        wndptr->mouseMove(GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ));
        break;
    case WM_PAINT:
        wndptr->paint();
        break;

    case WM_DESTROY:
        wndptr->cleanUp();

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
    wincl.lpfnWndProc = CanvasProcedure;
    wincl.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
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
                                 WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                                 0, 0, window_width, window_height,
                                 hWndParent,        /* The window is a child-window */
                                 NULL,
                                 GetModuleHandle(NULL),       /* Program Instance handler */
                                 this                 // object reference
                             );
    ShowWindow (hwnd, SW_SHOWDEFAULT);

    return hwnd;
}
