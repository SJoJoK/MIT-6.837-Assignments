#pragma once
#include "ray.h"
#include "hit.h"
#include "material.h"
#include "matrix.h"
#include "boundingbox.h"
#include "marchingInfo.h"
#include <vector>

class Grid;
class Object3D
{
protected:
    BoundingBox *boundingBox = nullptr;

public:
    Material *material;
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
    Grid* grid;

public:
    Group();
    Group(int n);
    void addObject(int index, Object3D *obj);
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
    Sphere();
    Sphere(Vec3f c, double r, Material *m);
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
    virtual void insertIntoGrid(Grid *g, Matrix *m);
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
    Triangle();
    Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m);
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
    Transform(Matrix &m, Object3D *o);
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
    vector<vector<Object3D *>> objs;
    bool visualize;

public:
    Grid(){};
    Grid(BoundingBox *bb, int nx, int ny, int nz) : nx(nx), ny(ny), nz(nz)
    {
        this->boundingBox = bb;
        m_is_voxel_opaque.assign(nx * ny * nz, false);
        objs.resize(nx * ny * nz);
    }
    BoundingBox *getBoundingBox();
    Vec3f getGird();
    void initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const;
    void insertIntoThis(int i, bool v, Object3D *obj);
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    virtual void paint();


};