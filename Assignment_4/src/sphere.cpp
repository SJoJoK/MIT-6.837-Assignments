#include "object3d.h"
#include "iostream"
extern int theta_steps;
extern int phi_steps;
extern bool gouraud;
const double PI = 3.1415926;
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
    glBegin(GL_QUADS);
    this->material->glSetMaterial();
    glPushMatrix();
    glutSolidSphere(radius, 100, 100);
    // glTranslatef(center.x(), center.y(), center.z());
    for (int i = 0; i < theta_steps; i++)
    {
        float phi = 0;
        float theta_next = theta + dtheta;
        Vec3f pt0 = getSphereCoord(theta, phi);
        Vec3f pt1 = getSphereCoord(theta_next, phi);
        Vec3f pt2 = getSphereCoord(theta_next, phi + dphi);
        Vec3f pt3 = getSphereCoord(theta, phi + dphi);
        for (int j = 0; j < phi_steps ; j++)
        {
            if (j != 0)
            {
                pt0 = pt3;
                pt1 = pt2;
                pt2 = getSphereCoord(theta_next, phi + dphi);
                pt3 = getSphereCoord(theta, phi + dphi);
            }
            Vec3f normal;
            //Normals of Vertexs
            if (gouraud)
            {
                normal = pt0 - this->center;
                normal.Normalize();
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(pt0.x(), pt0.y(), pt0.z());
                normal = pt1 - this->center;
                normal.Normalize();
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(pt1.x(), pt1.y(), pt1.z());
                normal = pt2 - this->center;
                normal.Normalize();
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(pt2.x(), pt2.y(), pt2.z());
                normal = pt3 - this->center;
                normal.Normalize();
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(pt3.x(), pt3.y(), pt3.z());
            }
            else
            {
                Vec3f a = pt3 - pt0;
                Vec3f b = pt1 - pt0;
                Vec3f::Cross3(normal, b, a);
                normal.Normalize();
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(pt0.x(), pt0.y(), pt0.z());
                glVertex3f(pt1.x(), pt1.y(), pt1.z());
                glVertex3f(pt2.x(), pt2.y(), pt2.z());
                glVertex3f(pt3.x(), pt3.y(), pt3.z());
                cout << "Sending..." << endl
                     << "P0:(" << pt0.x() << ", " << pt0.y() << ", " << pt0.z() << ")" << endl
                     << "P1:(" << pt1.x() << ", " << pt1.y() << ", " << pt1.z() << ")" << endl
                     << "P2:(" << pt2.x() << ", " << pt2.y() << ", " << pt2.z() << ")" << endl
                     << "P3:(" << pt3.x() << ", " << pt3.y() << ", " << pt3.z() << ")" << endl;
            }
            phi += dphi;
        }
        theta += dtheta;
    }
    glPopMatrix();
    glEnd();
}