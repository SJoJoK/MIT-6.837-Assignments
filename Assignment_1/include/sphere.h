#pragma once
#include "object3d.h"
class Sphere : public Object3D
{
private:
    Vec3f center;
    double radius;

public:
    Sphere() : center(Vec3f(0,0,0)), radius(0){};
    Sphere(Vec3f c, double r, Material *m) : Object3D(m), center(c), radius(r){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
};
