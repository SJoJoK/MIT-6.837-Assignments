#include "object3d.h"
#include "iostream"
extern int theta_steps;
extern int phi_steps;
extern bool gouraud;
const double PI = 3.1415926;
Sphere::Sphere()
{
    center = Vec3f(0, 0, 0);
    radius = 0;
}

Sphere::Sphere(Vec3f c, double r, Material *m) : Object3D(m), center(c), radius(r)
{
    this->getBoundingBox();
};

void Sphere::insertIntoGrid(Grid *g, Matrix *m)
{
    if (m != nullptr)
    {
        this->boundingBox = (new Transform(*m, this))->getBoundingBox();
        Vec3f m_min = boundingBox->getMin();
        Vec3f m_max = boundingBox->getMax();
        Vec3f v = g->getGird();
        BoundingBox *bb = g->getBoundingBox();
        Vec3f min = bb->getMin();
        Vec3f max = bb->getMax();
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

        if (_end_i >= x)
        {
            _end_i = x - 1;
        }

        if (_end_j >= y)
        {
            _end_j = y - 1;
        }
        if (_end_k >= z)
        {
            _end_k = z - 1;
        }
        for (int _i = _start_i; _i <= _end_i; _i++)
        {
            for (int _j = _start_j; _j <= _end_j; _j++)
            {
                for (int _k = _start_k; _k <= _end_k; _k++)
                {
                    g->insertIntoThis((_i * y + _j) * z + _k, true, this);
                }
            }
        }
        return;
    }
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
    float diag = sqrt(pow(grid_x, 2) + pow(grid_y, 2) + pow(grid_z, 2));
    Vec3f cen = center - min;
    Vec3f _voxel;
    for (int _i = 0; _i < x; _i++)
    {
        for (int _j = 0; _j < y; _j++)
        {
            for (int _k = 0; _k < z; _k++)
            {
                float _x1 = (_i + 0.5f) * grid_x;
                float _y1 = (_j + 0.5f) * grid_y;
                float _z1 = (_k + 0.5f) * grid_z;
                _voxel.Set(_x1, _y1, _z1);
                if ((_voxel - cen).Length() <= radius + 0.5 * diag)
                {
                    g->insertIntoThis((_i * y + _j) * z + _k, true, this);
                }
            }
        }
    }
}
BoundingBox *Sphere::getBoundingBox()
{
    if (this->boundingBox)
        return this->boundingBox;
    this->boundingBox = new BoundingBox(Vec3f(center.x() - radius, center.y() - radius, center.z() - radius),
                                        Vec3f(center.x() + radius, center.y() + radius, center.z() + radius));
    return this->boundingBox;
}
Vec3f Sphere::getSphereCoord(float theta, float phi)
{
    float x = radius * sin(theta) * cos(phi);
    float y = radius * sin(theta) * sin(phi);
    float z = radius * cos(theta);
    return Vec3f(x, y, z);
}
bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f Rd = r.getDirection();
    Vec3f Ro = r.getOrigin() - this->center;
    double a = Rd.Length() * Rd.Length();
    double b = 2 * Rd.Dot3(Ro);
    double c = Ro.Dot3(Ro) - pow(this->radius, 2);
    double d_2 = b * b - 4 * a * c;
    if (d_2 < 0)
        return false;
    double d = sqrt(d_2);
    double t_p = (-1 * b + d) / (2 * a);
    double t_n = (-1 * b - d) / (2 * a);
    if (t_p < 0)
        return false;
    double tmp = h.getT();
    if (t_n < 0)
    {
        if (t_p < tmp && t_p > tmin)
        {
            Vec3f h_pt = Ro + t_p * Rd;
            Vec3f normal = h_pt;
            normal.Normalize();
            h.set(t_p, this->material, normal, r);
            return true;
        }
    }
    else
    {
        if (t_n < tmp && t_n > tmin)
        {
            Vec3f h_pt = Ro + t_n * Rd;
            Vec3f normal = h_pt;
            normal.Normalize();
            h.set(t_n, this->material, normal, r);
            return true;
        }
    }
    return false;
}
void Sphere::paint()
{
    //P(r, theta, phi)
    //theta 0-PI
    //phi 0-2PI
    float dtheta = PI / (float)theta_steps;
    float dphi = 2 * PI / (float)phi_steps;
    float theta = 0;
    // glBegin(GL_QUADS);
    this->material->glSetMaterial();
    glPushMatrix();
    cout << center << endl;
    glTranslatef(center.x(), center.y(), center.z());
    glutSolidSphere(radius, 100, 100);
    // for (int i = 0; i < theta_steps; i++)
    // {
    //     float phi = 0;
    //     float theta_next = theta + dtheta;
    //     Vec3f pt0 = getSphereCoord(theta, phi);
    //     Vec3f pt1 = getSphereCoord(theta_next, phi);
    //     Vec3f pt2 = getSphereCoord(theta_next, phi + dphi);
    //     Vec3f pt3 = getSphereCoord(theta, phi + dphi);
    //     for (int j = 0; j < phi_steps ; j++)
    //     {
    //         if (j != 0)
    //         {
    //             pt0 = pt3;
    //             pt1 = pt2;
    //             pt2 = getSphereCoord(theta_next, phi + dphi);
    //             pt3 = getSphereCoord(theta, phi + dphi);
    //         }
    //         Vec3f normal;
    //         //Normals of Vertexs
    //         if (gouraud)
    //         {
    //             normal = pt0 - this->center;
    //             normal.Normalize();
    //             glNormal3f(normal.x(), normal.y(), normal.z());
    //             glVertex3f(pt0.x(), pt0.y(), pt0.z());
    //             normal = pt1 - this->center;
    //             normal.Normalize();
    //             glNormal3f(normal.x(), normal.y(), normal.z());
    //             glVertex3f(pt1.x(), pt1.y(), pt1.z());
    //             normal = pt2 - this->center;
    //             normal.Normalize();
    //             glNormal3f(normal.x(), normal.y(), normal.z());
    //             glVertex3f(pt2.x(), pt2.y(), pt2.z());
    //             normal = pt3 - this->center;
    //             normal.Normalize();
    //             glNormal3f(normal.x(), normal.y(), normal.z());
    //             glVertex3f(pt3.x(), pt3.y(), pt3.z());
    //         }
    //         else
    //         {
    //             Vec3f a = pt3 - pt0;
    //             Vec3f b = pt1 - pt0;
    //             Vec3f::Cross3(normal, b, a);
    //             normal.Normalize();
    //             glNormal3f(normal.x(), normal.y(), normal.z());
    //             glVertex3f(pt0.x(), pt0.y(), pt0.z());
    //             glVertex3f(pt1.x(), pt1.y(), pt1.z());
    //             glVertex3f(pt2.x(), pt2.y(), pt2.z());
    //             glVertex3f(pt3.x(), pt3.y(), pt3.z());
    //         }
    //         phi += dphi;
    //     }
    //     theta += dtheta;
    // }
    glPopMatrix();
    // glEnd();
}