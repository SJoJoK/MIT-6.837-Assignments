#pragma once
#include "spline.h"
#include "curve.h"
class Surface:public Spline
{
public:
    Curve *c;
    Surface(){};
    Surface(Curve *c) : c(c){};
    void set(int i, Vec3f v)
    {
        points[i] = v;
    }
};
class SurfaceOfRevolution:public Surface
{
public:
    SurfaceOfRevolution(){};
    SurfaceOfRevolution(Curve *c) : Surface(c){};
    TriangleMesh *OutputTriangles(ArgParser *args);
};
class BezierPatch:public Surface
{
public:
    BezierPatch()
    {
        c = new BezierCurve(4);
        points.resize(16);
    };
    BezierPatch(Curve *c) : Surface(c)
    {
        points.resize(16);
    };
    TriangleMesh *OutputTriangles(ArgParser *args);
};