#include "filter.h"
Vec3f Filter::getColor(int i, int j, Film *film)
{
    Vec3f color(0, 0, 0);
    Sample sample;
    int num_samples = film->getNumSamples();
    int support_radius = this->getSupportRadius();
    float weights = 0;
    float weight = 0;
    for (int _i = i - support_radius; _i <= i + support_radius; _i++)
    {
        if (_i < 0 || _i >= film->getWidth())
            continue;
        for (int _j = j - support_radius; _j <= j + support_radius; _j++)
        {
            if (_j < 0 || _j >= film->getHeight())
                continue;
            for (int n = 0; n < num_samples; n++)
            {
                sample = film->getSample(_i, _j, n);
                weight = getWeight(sample.getPosition().x() - 0.5 + (_i - i),
                                   sample.getPosition().y() - 0.5 + (_j - j));
                weights += weight;
                color += weight * sample.getColor();
            }
        }
    }
    color /= weights;
    return color;
}

float BoxFilter::getWeight(float x, float y)
{
    if (x >= radius || x <= -radius || y >= radius || y <= -radius)
        return 0;
    return 1;
}

float TentFilter::getWeight(float x, float y)
{
    Vec2f p(x, y);
    float pl = p.Length();
    if (pl >= radius)
        return 0;
    return (1 - pl / radius);
}

float GaussianFilter::getWeight(float x, float y)
{
    Vec2f p(x, y);
    float pl = p.Length();
    if (pl >= 2*sigma)
        return 0;
    return pow(e, -1 * pl * pl / (2 * sigma * sigma));
}
