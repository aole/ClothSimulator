#include "segment.h"


#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/polygon.hpp>

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
