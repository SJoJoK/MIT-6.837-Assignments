#include "sphere.h"
bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f Rd = r.getDirection();
    Vec3f Ro = r.getOrigin() - this->center;
    // double a = 1;
    //从Assignment 2 回来， 这里坑实在太大了...千万别默认！
    double b = 2 * Rd.Dot3(Ro);
    double c = Ro.Dot3(Ro) - pow(this->radius, 2);
    double d_2 = b * b - 4 * c;
    if (d_2 < 0)
        return false;
    double d = sqrt(d_2);
    double t_p = (-1 * b + d) / 2;
    double t_n = (-1 * b - d) / 2;
    if (t_p < 0)
        return false;
    double tmp = h.getT();
    if (t_n < 0)
    {
        if (t_p < tmp && t_p > tmin)
        {
            h.set(t_p, this->material, r);
            return true;
        }
    }
    else
    {
        if (t_n < tmp && t_n > tmin)
        {
            h.set(t_n, this->material, r);
            return true;
        }
    }
    return false;
}