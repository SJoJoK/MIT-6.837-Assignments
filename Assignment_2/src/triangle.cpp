#include "object3d.h"
bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f e1 = b - a;
    Vec3f e2 = c - a;
    Vec3f s = r.getOrigin() - a;
    Vec3f d = r.getDirection();
    Vec3f s1;
    Vec3f::Cross3(s1, d, e2);
    Vec3f s2;
    Vec3f::Cross3(s2, s, e1);
    float det = e1.Dot3(s1);
    if (det == 0)
        return false;
    else
    {
        float t = s2.Dot3(e2) / det;
        float b1 = s1.Dot3(s) / det;
        float b2 = s2.Dot3(d) / det;
        if (t > max(0.1f, tmin) && b1 + b2 <= 1 && b1 >= 0 && b2 >= 0 && t < h.getT())
        {
            h.set(t, this->material, this->normal, r);
            return true;
        }
    }
    return false;
}