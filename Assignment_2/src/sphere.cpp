#include "object3d.h"
bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f Rd = r.getDirection();
    Vec3f Ro = r.getOrigin() - this->center;
    double a = Rd.Length() * Rd.Length();
    double b = 2 * Rd.Dot3(Ro);
    double c = Ro.Dot3(Ro) - pow(this->radius, 2);
    double d_2 = b * b - 4 * a * c;
    if (d_2 < 0)
        return false;
    double d = sqrt(d_2);
    double t_p = (-1 * b + d) / (2 * a);
    double t_n = (-1 * b - d) / (2 * a);
    if (t_p < 0)
        return false;
    double tmp = h.getT();
    if (t_n < 0)
    {
        if (t_p < tmp && t_p > tmin)
        {
            Vec3f h_pt = Ro + t_p * Rd;
            Vec3f normal = h_pt;
            normal.Normalize();
            h.set(t_p, this->material, normal, r);
            return true;
        }
    }
    else
    {
        if (t_n < tmp && t_n > tmin)
        {
            Vec3f h_pt = Ro + t_n * Rd;
            Vec3f normal = h_pt;
            normal.Normalize();
            h.set(t_n, this->material, normal, r);
            return true;
        }
    }
    return false;
}