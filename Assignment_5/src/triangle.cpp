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

void Triangle::insertIntoGrid(Grid *g, Matrix *m)
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