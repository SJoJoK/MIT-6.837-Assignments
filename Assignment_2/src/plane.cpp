#include "object3d.h"
bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f Rd = r.getDirection();
    Vec3f Ro = r.getOrigin();
    float t = -1.0f * (distance + Ro.Dot3(this->normal)) / Rd.Dot3(this->normal);
    if (t > max(tmin,0.0f) && t < h.getT())
    {
        h.set(t, this->material, this->normal, r);
        return true;
    }
    return false;
}