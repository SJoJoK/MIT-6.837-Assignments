#include "object3d.h"
extern int nx, ny, nz;
extern bool is_grid;
Group::Group()
{
    n_objs = 0;
    boundingBox = nullptr;
}

Group::Group(int n)
{
    n_objs=n;
    objs = new Object3D *[n];
    boundingBox = nullptr;
}
void Group::addObject(int index, Object3D *obj)
{
    assert(index < n_objs);
    objs[index] = obj;
}

BoundingBox *Group::getBoundingBox()
{
    if (this->boundingBox)
        return this->boundingBox;
    float x_min = MAXFLOAT, y_min = MAXFLOAT, z_min = MAXFLOAT;
    float x_max = -1 * MAXFLOAT, y_max = -1 * MAXFLOAT, z_max = -1 * MAXFLOAT;
    for (int i = 0; i < this->n_objs; i++)
    {
        if (objs[i] == nullptr)
            continue;
        BoundingBox *bbox = objs[i]->getBoundingBox();
        if (bbox == nullptr)
            continue;
        Vec3f _v1 = bbox->getMax();
        Vec3f _v2 = bbox->getMin();
        float _x1 = _v1.x(), _y1 = _v1.y(), _z1 = _v1.z();
        float _x2 = _v2.x(), _y2 = _v2.y(), _z2 = _v2.z();
        x_min = min<float>(x_min, _x2);
        y_min = min<float>(y_min, _y2);
        z_min = min<float>(z_min, _z2);
        x_max = max<float>(x_max, _x1);
        y_max = max<float>(y_max, _y1);
        z_max = max<float>(z_max, _z1);
    }
    this->boundingBox = new BoundingBox(Vec3f(x_min, y_min, z_min), Vec3f(x_max, y_max, z_max));
    return this->boundingBox;
}
bool Group::intersect(const Ray &r, Hit &h, float tmin)
{
    bool res = false;
    if (is_grid)
    {
        res = grid->intersect(r, h, tmin);
    }
    else
    {
        for (int i = 0; i < this->n_objs; i++)
        {
            if (objs[i] == nullptr)
                continue;
            res = objs[i]->intersect(r, h, tmin) || res;
        }
    }
    return res;
}
bool Group::intersectShadowRay(const Ray &r, Hit &h, float tmin)
{
    bool res = false;
    if(is_grid)
    {
        res = grid->intersect(r, h, tmin);
    }
    else
    {
        for (int i = 0; i < this->n_objs; i++)
        {
            if (objs[i] == nullptr)
                continue;
            res = objs[i]->intersect(r, h, tmin);
            if (res)
                return true;
        }
    }
    return res;
}
void Group::paint()
{
    for (int i = 0; i < this->n_objs; i++)
    {
        if (objs[i] == nullptr)
            continue;
        objs[i]->paint();
    }
}
void Group::insertIntoGrid(Grid *g, Matrix *m)
{
    grid = g;
    for (int i = 0; i < n_objs; i++)
    {
        objs[i]->insertIntoGrid(g, m);
    }
}