#pragma once
#include "arg_parser.h"
#include "vectors.h"
#include "triangle_mesh.h"
class Spline
{
public:
    // FOR VISUALIZATION
    virtual void Paint(ArgParser *args){};

    // FOR CONVERTING BETWEEN SPLINE TYPES
    virtual void OutputBezier(FILE *file) {};
    virtual void OutputBSpline(FILE *file){};

    // FOR CONTROL POINT PICKING
    virtual int getNumVertices(){};
    virtual Vec3f getVertex(int i){};

    // FOR EDITING OPERATIONS
    virtual void moveControlPoint(int selectedPoint, float x, float y){};
    virtual void addControlPoint(int selectedPoint, float x, float y){};
    virtual void deleteControlPoint(int selectedPoint){};

    // FOR GENERATING TRIANGLES
    virtual TriangleMesh *OutputTriangles(ArgParser *args){};

    virtual void set(int i, Vec3f v){};
};