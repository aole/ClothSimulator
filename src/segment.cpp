#include "segment.h"

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
