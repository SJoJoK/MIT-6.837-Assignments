#pragma once
#include "matrix.h"
#include "ray.h"

class Camera
{
public:
    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;
    
};