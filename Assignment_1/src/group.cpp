#include "group.h"
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