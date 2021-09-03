#pragma once
#include"spline.h"
class Curve:public Spline
{
public:
    int num_p;
    Matrix B;
    Curve(){};
    Curve(int num_p) : num_p(num_p)
    {
        points.resize(num_p);
    };
    void set(int i, Vec3f v)
    {
        points[i] = v;
    }
    virtual void Paint(ArgParser *args);
    virtual void moveControlPoint(int selectedPoint, float x, float y)
    {
        points[selectedPoint] = Vec3f(x, y, 0);
    };
    virtual void addControlPoint(int selectedPoint, float x, float y)
    {
        points.insert(points.begin() + selectedPoint, Vec3f(x, y, 0));
        num_p++;
    };
    virtual void deleteControlPoint(int selectedPoint)
    {
        points.erase(points.begin() + selectedPoint);
        num_p--;
    };
    virtual int getNumVertices()
    {
        return num_p;
    };
    virtual Vec3f getVertex(int i)
    {
        return points[i];
    }
    Vec3f GBT(int i, float t); 
};
class BezierCurve:public Curve
{
public:
    BezierCurve(int num_p) : Curve(num_p)
    {
        B = Matrix((const float[])
            {
            -1, 3, -3, 1, 
            3, -6, 3, 0, 
            -3, 3, 0, 0, 
            1, 0, 0, 0
            }
        );
    };
    void Paint(ArgParser *args);
    void OutputBezier(FILE *file);
    void OutputBSpline(FILE *file);
};
class BSplineCurve:public Curve
{
public:
    BSplineCurve(int num_p) : Curve(num_p)
    {
        B = Matrix((const float[]){
            -1, 3, -3, 1,
            3, -6, 0, 4,
            -3, 3, 3, 1,
            1, 0, 0, 0});
        B *= (1 / 6.0f);
    };
    void Paint(ArgParser *args);
    void OutputBezier(FILE *file);
    void OutputBSpline(FILE *file);
};
