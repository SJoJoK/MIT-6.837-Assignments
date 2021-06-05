#include "object3d.h"
bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
    //NP + D = 0
    Vec3f Rd = r.getDirection();
    Vec3f Ro = r.getOrigin();
    float t = -1.0f * (distance + Ro.Dot3(this->normal)) / Rd.Dot3(this->normal);
    if (t > max(tmin, 0.0f) && t < h.getT())
    {
        h.set(t, this->material, this->normal, r);
        return true;
    }
    return false;
}
void Plane ::paint()
{
    Vec3f d1, d2, v(1, 0, 0);
    if (normal.Dot3(v) == 0)
    {
        v = Vec3f(0, 1, 0);
    }
    Vec3f::Cross3(d1, v, normal);
    d1.Normalize();
    Vec3f::Cross3(d2, normal, d1);
    d2.Normalize();
    d1 *= 10000;
    d2 *= 10000;
    Vec3f e1 = d1 + d2, e2 = d1 * (-1) + d2, e3 = d1 * (-1) - d2, e4 = d1 - d2;
    e1 -= distance * normal;
    e2 -= distance * normal;
    e3 -= distance * normal;
    e4 -= distance * normal;
    material->glSetMaterial();
    glBegin(GL_QUADS);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(e1.x(), e1.y(), e1.z());
    glVertex3f(e2.x(), e2.y(), e2.z());
    glVertex3f(e3.x(), e3.y(), e3.z());
    glVertex3f(e4.x(), e4.y(), e4.z());
    glEnd();
}