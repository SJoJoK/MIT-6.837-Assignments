#pragma once
#include "vectors.h"
#include "sample.h"
#include "film.h"
class Filter
{
public:
    Filter();
    Vec3f getColor(int i, int j, Film *film);
    virtual float getWeight(float x, float y) = 0;
    virtual int getSupportRadius() = 0;
    ~Filter();
};

class BoxFilter:public Filter
{
private:
    float radius = 0.5;

public:
    BoxFilter(float radius)
    {
        this->radius = radius;
    }
    float getWeight(float x, float y);
    int getSupportRadius()
    {
        return floor(radius + 0.5);
    }
};