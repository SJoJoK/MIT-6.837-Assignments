#pragma once
#include "vectors.h"
#include "sample.h"
#include "film.h"
class Filter
{
public:
    Filter(){};
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

class TentFilter : public Filter
{
private:
    float radius = 0.5;

public:
    TentFilter(float radius)
    {
        this->radius = radius;
    }
    float getWeight(float x, float y);
    int getSupportRadius()
    {
        return floor(radius + 0.5);
    }
};

class GaussianFilter : public Filter
{
private:
    float sigma = 0.25;
    float e = 2.718281828459045;

public:
    GaussianFilter(float sigma)
    {
        this->sigma = sigma;
    }
    float getWeight(float x, float y);
    int getSupportRadius()
    {
        return floor(2 * sigma + 0.5);
    }
};