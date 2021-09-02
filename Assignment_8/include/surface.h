#pragma once
#include"spline.h"
class Surface:public Spline
{
public:
    Curve *c;
    Surface(){};
    Surface(Curve *c) : c(c){};
};
class SurfaceOfRevolution:public Surface
{
public:
    SurfaceOfRevolution(){};
    SurfaceOfRevolution(Curve *c) : Surface(c){};
};
class BezierPatch:public Surface
{
};