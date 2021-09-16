#pragma once
#include "matrix.h"
#include "ray.h"

class Camera
{
protected:
    Vec3f center;
    Vec3f direction;
    Vec3f up;
    Vec3f horizontal;
    float ratio = 1.0; //it's width/height

public:
    Camera(){};
    Camera(Vec3f c, Vec3f dir, Vec3f up)
    {
        this->center = c;
        this->direction = dir;
        this->direction.Normalize();
        Vec3f::Cross3(this->horizontal, dir, up);
        this->horizontal.Normalize();
        Vec3f::Cross3(this->up, this->horizontal, this->direction);
    }
    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;
};

class OrthographicCamera : public Camera
{
private:
    float size;

public:
    OrthographicCamera(){};
    OrthographicCamera(Vec3f c, Vec3f dir, Vec3f up, float sz) : Camera(c, dir, up), size(sz) {}
    virtual Ray generateRay(Vec2f point);
    virtual float getTMin() const;
};

class PerspectiveCamera : public Camera
{
private:
    float fov;

public:
    PerspectiveCamera(){};
    PerspectiveCamera(Vec3f c, Vec3f dir, Vec3f up, float angle) : Camera(c, dir, up), fov(angle) {}
    virtual Ray generateRay(Vec2f point);
    virtual float getTMin() const;
};