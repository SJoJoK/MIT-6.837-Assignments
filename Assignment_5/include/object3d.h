#pragma once
#include "ray.h"
#include "hit.h"
#include "material.h"
#include "matrix.h"
#include "boundingbox.h"
#include <vector>

class Grid;
class Object3D
{
protected:
    Material *material;
    BoundingBox *boundingBox;

public:
    bool _isTriangle = false;
    Vec3f _a;
    Vec3f _b;
    Vec3f _c;

public:
    Object3D() : material(nullptr){};
    Object3D(Material *m) : material(m){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
    virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin)
    {
        return this->intersect(r, h, tmin);
    }
    virtual void paint(void) = 0;
    virtual BoundingBox *getBoundingBox()
    {
        boundingBox = nullptr;
        return boundingBox;
    };
    virtual void insertIntoGrid(Grid *g, Matrix *m){};
    virtual ~Object3D(){};
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
    virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin);
    virtual BoundingBox *getBoundingBox();
    virtual void insertIntoGrid(Grid *g, Matrix *m);
    virtual void paint();
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

class Sphere : public Object3D
{
private:
    Vec3f center;
    double radius;

public:
    Sphere() : center(Vec3f(0, 0, 0)), radius(0){};
    Sphere(Vec3f c, double r, Material *m) : Object3D(m), center(c), radius(r){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    virtual void paint();
    virtual BoundingBox *getBoundingBox();
    virtual void insertIntoGrid(Grid *g, Matrix *m);
    Vec3f getSphereCoord(float theta, float phi);
};

class Plane : public Object3D
{
private:
    Vec3f normal;
    float distance;

public:
    Plane(){};
    //np + d = 0
    Plane(Vec3f &normal, float d, Material *m) : Object3D(m), normal(normal), distance(-1 * d){};
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    virtual void paint();
};

class Triangle : public Object3D
{
private:
    Vec3f normal;
    Vec3f a;
    Vec3f b;
    Vec3f c;

public:
    Triangle() { _isTriangle = true; };
    Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m) : Object3D(m), a(a), b(b), c(c)
    {
        Vec3f AB = b - a;
        Vec3f AC = c - a;
        Vec3f::Cross3(normal, AB, AC);
        normal.Normalize();
        _isTriangle = true;
        _a = a;
        _b = b;
        _c = c;
    }
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    virtual BoundingBox *getBoundingBox();
    virtual void insertIntoGrid(Grid *g, Matrix *m);
    virtual void paint();
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
    virtual BoundingBox *getBoundingBox();
    virtual void insertIntoGrid(Grid *g, Matrix *m);
    virtual void paint();
};

class Grid: public Object3D
{
private:
    int nx;
    int ny;
    int nz;
    vector<bool> m_is_voxel_opaque;
    vector<Object3D *> objs;
    bool visualize;

public:
    Grid(BoundingBox *bb, int nx, int ny, int nz):nx(nx),ny(ny),nz(nz)
    {
        this->boundingBox = bb;
        m_is_voxel_opaque.assign(nx * ny * nz, false);
        objs.resize(nx * ny * nz);
    }
    BoundingBox *getBoundingBox();
    Vec3f getGird();
    void insertIntoThis(int i, bool v, Object3D *obj);
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    virtual void paint();
    void setMaterial(Material *m) { this->material = m; };


};