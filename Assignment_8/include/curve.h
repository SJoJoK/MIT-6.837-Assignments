#pragma once
#include"spline.h"
class Curve:public Spline
{
public:
    int num_v;
    Curve(){};
    Curve(int num_v) : num_v(num_v){};
};
class BezierCurve:public Curve
{
public:
    BezierCurve(int num_v) : Curve(num_v){};
};
class BSplineCurve:public Curve
{
public:
    BSplineCurve(int num_v) : Curve(num_v){};
};
