#include "object3d.h"
bool Transform::intersect(const Ray &r, Hit &h, float tmin)
{
    bool res = false;
    Matrix tmp = this->transform_mat;
    if (tmp.Inverse())
    {
        Vec3f dir = r.getDirection();
        Vec3f o = r.getOrigin();
        tmp.Transform(o);
        tmp.TransformDirection(dir);
        //Without Normalization
        Ray nr(o, dir);
        res = obj->intersect(nr, h, tmin);
        if (res)
        {
            Vec3f n = h.getNormal();
            tmp.Transpose();
            tmp.TransformDirection(n);
            n.Normalize();
            h.set(h.getT(), h.getMaterial(), n, r);
        }
    }
    return res;
}
void Transform::paint()
{
    glPushMatrix();
    GLfloat *glMatrix = this->transform_mat.glGet();
    glMultMatrixf(glMatrix);
    delete[] glMatrix;
    this->obj->paint();
    glPopMatrix();
}