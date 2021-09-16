#pragma once
#include "camera.h"
class OrthographicCamera : public Camera
{
private:
    Vec3f center;
    Vec3f direction;
    Vec3f up;
    Vec3f horizontal;
    float size;

public:
    OrthographicCamera(){};
    OrthographicCamera(Vec3f c, Vec3f dir, Vec3f up, double sz)
    {
        this->center = c;
        this->direction = dir;
        this->direction.Normalize();
        Vec3f::Cross3(this->horizontal, dir, up);
        this->horizontal.Normalize();
        Vec3f::Cross3(this->up, this->horizontal, this->direction);
        this->size = sz;
    }
    virtual Ray generateRay(Vec2f point);
    virtual float getTMin() const;
};