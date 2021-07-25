#include "object3d.h"
inline float MIN3(float x,float y,float z)
{
    float tmp = x < y ? x : y;
    return tmp < z ? tmp : z;
}
inline float MAX3(float x, float y, float z)
{
    float tmp = x > y ? x : y;
    return tmp > z ? tmp : z;
}
BoundingBox *Triangle::getBoundingBox()
{
    if (this->boundingBox)
        return this->boundingBox;
    this->boundingBox = new BoundingBox(Vec3f(MIN3(a.x(), b.x(), c.x()), MIN3(a.y(), b.y(), c.y()), MIN3(a.z(), b.z(), c.z())), 
                                Vec3f(MAX3(a.x(), b.x(), c.x()), MAX3(a.y(), b.y(), c.y()), MAX3(a.z(), b.z(), c.z())));
    return this->boundingBox;
}

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
        if (t > max(0.0f, tmin) && b1 + b2 <= 1 && b1 >= 0 && b2 >= 0 && t < h.getT())
        {
            h.set(t, this->material, this->normal, r);
            return true;
        }
    }
    return false;
}
void Triangle::paint()
{
    material->glSetMaterial();
    glBegin(GL_TRIANGLES);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(a.x(), a.y(), a.z());
    glVertex3f(b.x(), b.y(), b.z());
    glVertex3f(c.x(), c.y(), c.z());
    glEnd();
}