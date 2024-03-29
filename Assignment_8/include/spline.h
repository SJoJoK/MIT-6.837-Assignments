#pragma once
#include "vector"
#include "arg_parser.h"
#include "vectors.h"
#include "matrix.h"
#include "triangle_mesh.h"
class Spline
{
public:
    vector<Vec3f> points;
    // FOR VISUALIZATION
    virtual void Paint(ArgParser *args){};

    // FOR CONVERTING BETWEEN SPLINE TYPES
    virtual void OutputBezier(FILE *file){};
    virtual void OutputBSpline(FILE *file){};

    // FOR CONTROL POINT PICKING
    virtual int getNumVertices() { return 0; };
    virtual Vec3f getVertex(int i) { return Vec3f(0, 0, 0); };

    // FOR EDITING OPERATIONS
    virtual void moveControlPoint(int selectedPoint, float x, float y){};
    virtual void addControlPoint(int selectedPoint, float x, float y){};
    virtual void deleteControlPoint(int selectedPoint){};

    // FOR GENERATING TRIANGLES
    virtual TriangleMesh *OutputTriangles(ArgParser *args) { return nullptr; };

    virtual void set(int i, Vec3f v)
    {
        
    };
};