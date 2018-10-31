#include "segment.h"


#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <sstream>

typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> point_t;
typedef boost::geometry::model::polygon<point_t, FALSE, FALSE> polygon_type;

Vertex *Segment::splitAt(double Cx, double Cy)
{
    double Ax = getx(0);
    double Ay = gety(0);
    double Bx = getx(1);
    double By = gety(1);

    double t= -((Ax-Cx)*(Bx-Ax)+(Ay-Cy)*(By-Ay))/((Bx-Ax)*(Bx-Ax)+(By-Ay)*(By-Ay));
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
    for(Segment *s: m_segments){
        boost::geometry::append(poly.outer(), point_t(s->at(0)->m_x, s->at(0)->m_y));
    }

    return boost::geometry::within(point_t(p->m_x, p->m_y), poly);
}

void Shape::save(std::wofstream &stream)
{
    for(Vertex *v: m_vertices) {
        stream << L"vertex=" << v->m_x << " " << v->m_y << std::endl;
    }

    for(Segment *s: m_segments) {
        stream << L"segment=" << std::distance(m_vertices.begin(),std::find(m_vertices.begin(), m_vertices.end(), s->at(0))) << " ";
        stream << std::distance(m_vertices.begin(),std::find(m_vertices.begin(), m_vertices.end(), s->at(1))) << std::endl;
    }
}

void Shape::process(std::wstring key, std::wstring value)
{
    if(key==L"vertex"){
        std::wistringstream stm(value);
        double vx, vy;
        stm >> vx >> vy;

        Vertex *v = new Vertex(vx, vy, this);

        m_vertices.push_back(v);
    } else if(key==L"segment"){
        std::wistringstream stm(value);
        int a, b;
        stm >> a >> b;

        m_segments.push_back(new Segment(m_vertices[a], m_vertices[b], this));
    }
}
