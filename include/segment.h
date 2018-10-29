#ifndef SEGMENT_H
#define SEGMENT_H

#include <windows.h>
#include <vector>

class Shape;

class Vertex
{
public:
    double m_x;
    double m_y;
    Shape *shape;

    Vertex():m_x(0),m_y(0), shape(NULL) {}
    Vertex(double x, double y):m_x(x),m_y(y), shape(NULL) {}
    Vertex(double x, double y, Shape *s):m_x(x),m_y(y), shape(s) {}

    void set(double x, double y)
    {
        m_x=x;
        m_y=y;
    }
};

class Segment : public std::vector <Vertex*>
{
public:
    Shape *shape;

    Segment(Shape *s):shape(s) {}

    Segment(Vertex *a, Vertex *b, Shape *s)
    {
        push_back(a);
        push_back(b);
        shape = s;
    }

    double getx(int index)
    {
        return this->at(index)->m_x;
    }

    double gety(int index)
    {
        return this->at(index)->m_y;
    }

    void addPoint(double x, double y)
    {
        this->at(0)->m_x += x;
        this->at(0)->m_y += y;
        this->at(1)->m_x += x;
        this->at(1)->m_y += y;
    }
    void splitAt(double Cx, double Cy);
};

class Shape
{
public:
    std::vector<Vertex*> m_vertices;
    std::vector<Segment*> m_segments;

    Shape(Vertex &v1, Vertex &v2)
    {
        Vertex *a = new Vertex(v1.m_x,v1.m_y, this);
        Vertex *b = new Vertex(v2.m_x,v1.m_y, this);
        Vertex *c = new Vertex(v2.m_x,v2.m_y, this);
        Vertex *d = new Vertex(v1.m_x,v2.m_y, this);
        m_vertices.push_back(a);
        m_vertices.push_back(b);
        m_vertices.push_back(c);
        m_vertices.push_back(d);

        m_segments.push_back(new Segment(a, b, this));
        m_segments.push_back(new Segment(b, c, this));
        m_segments.push_back(new Segment(c, d, this));
        m_segments.push_back(new Segment(d, a, this));
    }

    void addVertex(Vertex *v)
    {
        m_vertices.push_back(v);
    }

    void addSegment(Segment *s)
    {
        m_segments.push_back(s);
    }

    virtual ~Shape()
    {
        for (Segment *s: m_segments)
            delete s;
        for (Vertex *v: m_vertices)
            delete v;
    }
};

#endif // SEGMENT_H
