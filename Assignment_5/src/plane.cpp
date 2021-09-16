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

void Plane::insertIntoGrid(Grid *g, Matrix *m)
{
    Vec3f m_min = boundingBox->getMin();
    Vec3f m_max = boundingBox->getMax();
    Vec3f v = g->getGird();
    BoundingBox *bb = g->getBoundingBox();
    Vec3f min = bb->getMin();
    Vec3f max = Vec3f(bb->getMax().x(), bb->getMax().y(), bb->getMax().z());
    int x = v.x();
    int y = v.y();
    int z = v.z();
    Vec3f size = max - min;
    float grid_x = size.x() / x;
    float grid_y = size.y() / y;
    float grid_z = size.z() / z;

    int _start_i = (fabs(m_min.x() - min.x())) * (1 / grid_x);
    int _start_j = (fabs(m_min.y() - min.y())) * (1 / grid_y);
    int _start_k = (fabs(m_min.z() - min.z())) * (1 / grid_z);
    int _end_i = (fabs(m_max.x() - min.x())) * (1 / grid_x);
    int _end_j = (fabs(m_max.y() - min.y())) * (1 / grid_y);
    int _end_k = (fabs(m_max.z() - min.z())) * (1 / grid_z);

    if (_start_i == _end_i)
        _end_i++;
    if (_start_j == _end_j)
        _end_j++;
    if (_start_k == _end_k)
        _end_k++;
    if (_start_i > _end_i)
        swap(_start_i, _end_i);
    if (_start_j > _end_j)
        swap(_start_j, _end_j);
    if (_start_k > _end_k)
        swap(_start_k, _end_k);

    assert(_end_i <= x && _end_j <= y && _end_k <= z);

    for (int _i = _start_i; _i < _end_i; _i++)
    {
        for (int _j = _start_j; _j < _end_j; _j++)
        {
            for (int _k = _start_k; _k < _end_k; _k++)
            {
                g->insertIntoThis((_i * y + _j) * z + _k, true, this);
            }
        }
    }
}