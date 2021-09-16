#pragma once
#include "vectors.h"
class Sampler
{
private:
    int sample_per_pixel = 0;

public:
    Sampler(int spp) : sample_per_pixel(spp){};
    virtual Vec2f getSamplePosition(int n) = 0;
    ~Sampler();
};

class RandomSampler : public Sampler
{
public:
    RandomSampler(int spp) : Sampler(spp){};
    Vec2f getSamplePosition(int n)
    {
        return Vec2f(rand() / double(RAND_MAX), rand() / double(RAND_MAX));
    }
};

class UniformSampler : public Sampler
{
private:
    int length = 1;
    float size = 1;

public:
    UniformSampler(int spp) : Sampler(spp) 
    { 
        length = sqrt(spp);
        size = 1.0f / length;
    };
    Vec2f getSamplePosition(int n)
    {
        int x = n % length;
        int y = (n / length) % length;
        return Vec2f(size * (0.5 + x), size * (0.5 + y));
    }
};

class JitteredSampler : public Sampler
{
private:
    int length = 1;
    float size = 1;

public:
    JitteredSampler(int spp) : Sampler(spp)
    {
        length = sqrt(spp);
        size = 1.0f / length;
    };
    Vec2f getSamplePosition(int n)
    {
        int x = n % length;
        int y = (n / length) % length;
        return Vec2f(size * (x + rand() / double(RAND_MAX)), size * (y + rand() / double(RAND_MAX)));
    }
};
