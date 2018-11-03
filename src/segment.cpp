#include "segment.h"


#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/foreach.hpp>

#include <sstream>

typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> point_t;
typedef boost::geometry::model::polygon<point_t, FALSE, FALSE> polygon_type;


vector<glm::vec3> vertices;

Shape::Shape(Vertex& v1, Vertex& v2)
{
    double minx = min(v1.m_x, v2.m_x);
    double maxx = max(v1.m_x, v2.m_x);
    double miny = min(v1.m_y, v2.m_y);
    double maxy = max(v1.m_y, v2.m_y);

    Vertex *a = new Vertex(minx, miny, this);
    Vertex *b = new Vertex(maxx, miny, this);
    Vertex *c = new Vertex(maxx, maxy, this);
    Vertex *d = new Vertex(minx, maxy, this);

    m_vertices.push_back(a);
    m_vertices.push_back(b);
    m_vertices.push_back(c);
    m_vertices.push_back(d);

    m_segments.push_back(new Segment(a, b, this));
    m_segments.push_back(new Segment(b, c, this));
    m_segments.push_back(new Segment(c, d, this));
    m_segments.push_back(new Segment(d, a, this));
}

Vertex *Segment::splitAt(Vertex C)
{
    double Ax = getx(0);
    double Ay = gety(0);
    double Bx = getx(1);
    double By = gety(1);

    double t= -((Ax-C.m_x)*(Bx-Ax)+(Ay-C.m_y)*(By-Ay))/((Bx-Ax)*(Bx-Ax)+(By-Ay)*(By-Ay));
    double Dx=Ax+t*(Bx-Ax);
    double Dy=Ay+t*(By-Ay);

    Vertex *nv = new Vertex(Dx,Dy, shape);

    shape->addVertex(nv);
    shape->addSegment(new Segment(nv, at(1), shape), this);

    at(1) = nv;

    return nv;
}

bool Shape::within(Vertex* p)
{
    polygon_type poly;
    for(Segment *s: m_segments)
    {
        boost::geometry::append(poly.outer(), point_t(s->at(0)->m_x, s->at(0)->m_y));
    }

    return boost::geometry::within(point_t(p->m_x, p->m_y), poly);
}

void Shape::save(std::wofstream &stream)
{
    for(Vertex *v: m_vertices)
    {
        stream << L"vertex=" << v->m_x << " " << v->m_y << std::endl;
    }

    for(Segment *s: m_segments)
    {
        stream << L"segment=" << std::distance(m_vertices.begin(),std::find(m_vertices.begin(), m_vertices.end(), s->at(0))) << " ";
        stream << std::distance(m_vertices.begin(),std::find(m_vertices.begin(), m_vertices.end(), s->at(1))) << std::endl;
    }
}

void Shape::process(std::wstring key, std::wstring value)
{
    if(key==L"vertex")
    {
        std::wistringstream stm(value);
        double vx, vy;
        stm >> vx >> vy;

        Vertex *v = new Vertex(vx, vy, this);

        m_vertices.push_back(v);
    }
    else if(key==L"segment")
    {
        std::wistringstream stm(value);
        int a, b;
        stm >> a >> b;

        m_segments.push_back(new Segment(m_vertices[a], m_vertices[b], this));
    }
}

void Shape::RenderGrid(HDC hdc)
{
    // create grid
    Vertex *x0, *y0, *xe, *ye;
    x0 = y0 = xe = ye = NULL;
    // find left most point = x0
    // find top most point = y0
    // find right most point = xe
    // find bottom most point = ye
    for(Vertex *v: m_vertices)
    {
        if(!x0)
        {
            x0 = y0 = xe = ye = v;
            continue;
        }
        if(v->m_x < x0->m_x)
            x0 = v;
        if(v->m_x > xe->m_x)
            xe = v;
        if(v->m_y < y0->m_y)
            y0 = v;
        if(v->m_y > ye->m_y)
            ye = v;
    }
    polygon_type main_shape;
    for(Segment *s: m_segments)
    {
        boost::geometry::append(main_shape.outer(), point_t(s->at(0)->m_x, s->at(0)->m_y));
    }

    // distance between points = d
    double d = 20;

    // find intersecting polygons between each cell and the main shape
    for(double y=y0->m_y; y<ye->m_y; y += d)
    {
        for(double x=x0->m_x; x<xe->m_x; x += d)
        {
            polygon_type cell;
            boost::geometry::append(cell.outer(), point_t(x, y));
            boost::geometry::append(cell.outer(), point_t(x+d, y));
            boost::geometry::append(cell.outer(), point_t(x+d, y+d));
            boost::geometry::append(cell.outer(), point_t(x, y+d));

            std::vector<polygon_type> intersected_polys;
            boost::geometry::intersection(main_shape, cell, intersected_polys);

            for(polygon_type poly: intersected_polys)
            {
                auto itb = boost::begin(boost::geometry::exterior_ring(poly));
                MoveToEx(hdc, (int)boost::geometry::get<0>(*itb), (int)boost::geometry::get<1>(*itb), NULL);
                for(auto it = boost::begin(boost::geometry::exterior_ring(poly))+1; it != boost::end(boost::geometry::exterior_ring(poly)); ++it)
                {
                    double px = boost::geometry::get<0>(*it);
                    double py = boost::geometry::get<1>(*it);
                    LineTo(hdc, (int)(px), (int)(py));
                }
            }
        }
    }
}

vector<glm::vec3> &Shape::getOpenGLVertices()
{
    return vertices;
}

