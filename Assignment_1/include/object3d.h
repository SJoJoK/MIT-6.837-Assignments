#pragma once
#include "ray.h"
#include "hit.h"
#include "material.h"
class Object3D
{
protected:
    Material *material;
public:
    Object3D() : material(nullptr){};
    Object3D(Material *m) : material(m){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
    virtual ~Object3D(){};
};