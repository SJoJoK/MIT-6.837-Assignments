#pragma once
#include "object3d.h"
#include <vector>
#include <array>
class Group : public Object3D
{
private:
    int n_objs;
    Object3D **objs;

public:
    Group() : n_objs(0){};
    Group(int n) : n_objs(n)
    {
        objs = new Object3D *[n];
    }
    void addObject(int index, Object3D *obj)
    {
        assert(index < n_objs);
        objs[index] = obj;
    }
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    ~Group()
    {
        //不知道要不要销毁对象...先不销毁吧
        for (int i = 0; i < n_objs; i++)
        {
            delete objs[i];
        }
        delete[] objs;
    }
};
