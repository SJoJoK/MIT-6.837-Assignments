#pragma once
#include "ray.h"
#include "hit.h"
#include "material.h"
#include "matrix.h"
#include <vector>
class Object3D
{
protected:
    Material *material;

public:
    Object3D() : material(nullptr){};
    Object3D(Material *m) : material(m){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
    virtual ~Object3D(){};
};

class Sphere : public Object3D
{
private:
    Vec3f center;
    double radius;

public:
    Sphere() : center(Vec3f(0, 0, 0)), radius(0){};
    Sphere(Vec3f c, double r, Material *m) : Object3D(m), center(c), radius(r){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
};

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
        //不知道要不要销毁对象...先销毁吧
        for (int i = 0; i < n_objs; i++)
        {
            delete objs[i];
        }
        delete[] objs;
    }
};

class Plane : public Object3D
{
private:
    Vec3f normal;
    float distance;

public:
    Plane(){};
    Plane(Vec3f &normal, float d, Material *m) : Object3D(m), normal(normal), distance(d){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
};

class Triangle : public Object3D
{
private:
    Vec3f normal;
    Vec3f a;
    Vec3f b;
    Vec3f c;

public:
    Triangle(){};
    Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m) : Object3D(m), a(a), b(b), c(c)
    {
        Vec3f AB = b - a;
        Vec3f AC = c - a;
        Vec3f::Cross3(normal, AB, AC);
        normal.Normalize();
    }
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
};

class Transform : public Object3D
{
private:
    Object3D *obj;
    Matrix transform_mat;

public:
    Transform(){};
    Transform(Matrix &m, Object3D *o) : transform_mat(m), obj(o){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
};