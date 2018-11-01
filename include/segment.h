#ifndef SEGMENT_H
#define SEGMENT_H

#include <windows.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

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

    void addPoint(double x, double y)
    {
        m_x += x;
        m_y += y;
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
    Vertex *splitAt(double Cx, double Cy);
};

class Shape
{
public:
    std::vector<Vertex*> m_vertices;
    std::vector<Segment*> m_segments;

    wstring getName() { return L"SHAPE"; }

    void save(std::wofstream &stream);

    Shape() {}

    Shape(Vertex &v1, Vertex &v2)
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

    void RenderGrid(HDC hdc);

    void process(std::wstring key, std::wstring value);

    void addVertex(Vertex *v)
    {
        m_vertices.push_back(v);
    }

    void addSegment(Segment *s)
    {
        m_segments.push_back(s);
    }

    void addSegment(Segment *s, Segment *after)
    {
        auto it = std::find(m_segments.begin(),m_segments.end(), after);
        m_segments.insert(it+1, s);
    }

    void translate(double x, double y) {
        for(Vertex *v: m_vertices)
            v->addPoint(x, y);
    }

    bool within(Vertex *p);

    void removeVertex(Vertex *v)
    {
        Segment *a = NULL;
        int ai = -1;
        Segment *b = NULL;
        int bi = -1;
        for(Segment *seg: m_segments)
        {
            if(seg->at(0)==v)
            {
                if (!a)
                {
                    a = seg;
                    ai = 1;
                }
                else
                {
                    b=seg;
                    bi=1;
                    break;
                }
            }
            else if(seg->at(1)==v)
            {
                if (!a)
                {
                    a = seg;
                    ai = 0;
                }
                else
                {
                    b=seg;
                    bi = 0;
                    break;
                }
            }
        }

        m_segments.push_back(new Segment(a->at(ai), b->at(bi), this));
        m_segments.erase(std::remove(m_segments.begin(), m_segments.end(), a), m_segments.end());
        m_segments.erase(std::remove(m_segments.begin(), m_segments.end(), b), m_segments.end());
        delete a;
        delete b;

        m_vertices.erase(std::remove(m_vertices.begin(), m_vertices.end(), v), m_vertices.end());
        delete v;
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
