#include "curve.h"
#include "matrix.h"
#include <GL/glut.h>
#include <GL/gl.h>
void Curve::Paint(ArgParser* args)
{
    //DRAW THE POLYGON
    glColor3f(0,0,1);
    glLineWidth(1);
    glBegin(GL_LINES);
    for (int i = 1; i < num_p; i++)
    {
        glVertex3f(points[i - 1].x(), points[i - 1].y(), points[i - 1].z());
        glVertex3f(points[i].x(), points[i].y(), points[i].z());
    }
    glEnd();

    //DRAW THE CONTROL POINTS
    glColor3f(1,1,1);
    glPointSize(5);
    glBegin(GL_POINTS);
    for (int i = 0; i < num_p; i++)
    {
        glVertex3f(points[i].x(), points[i].y(), points[i].z());
    }
    glEnd();

    //DRAW THE CURVE
    glColor3f(0.f, 1.f, 0.f);
    glLineWidth(1);
    glBegin(GL_LINES);
    float t = 0;
    float delta = 1.0f / args->curve_tessellation;
    Vec3f curve_pt;
    for (int i = 0; i <= args->curve_tessellation; i++)
    {
        curve_pt = GBT(i,t);
        glVertex3f(curve_pt[0], curve_pt[1], curve_pt[2]);
        t += delta;
    }
    glEnd();
}

Vec3f Curve::GBT(int i, float t)
{
    Vec4f T(pow(t, 3), pow(t, 2), pow(t, 1), 1);
    B.Transform(T);
    return points[i] * T[0] + points[i + 1] * T[1] + points[i + 2] * T[2] + points[i + 3] * T[3];
}