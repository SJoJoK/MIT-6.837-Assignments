#include "object3d.h"
bool Group::intersect(const Ray &r, Hit &h, float tmin)
{
    bool res = false;
    for (int i = 0; i < this->n_objs; i++)
    {
        if (objs[i] == nullptr)
            continue;
        res = objs[i]->intersect(r, h, tmin) || res;
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