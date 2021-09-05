#include "curve.h"
#include "matrix.h"
#include <GL/glut.h>
#include <GL/gl.h>
void Curve::Paint(ArgParser *args)
{
    //DRAW THE POLYGON
    glColor3f(0, 0, 1);
    glLineWidth(1);
    glBegin(GL_LINES);
    for (int i = 1; i < num_p; i++)
    {
        glVertex3f(points[i - 1].x(), points[i - 1].y(), points[i - 1].z());
        glVertex3f(points[i].x(), points[i].y(), points[i].z());
    }
    glEnd();

    //DRAW THE CONTROL POINTS
    glColor3f(1, 1, 1);
    glPointSize(5);
    glBegin(GL_POINTS);
    for (int i = 0; i < num_p; i++)
    {
        glVertex3f(points[i].x(), points[i].y(), points[i].z());
    }
    glEnd();
}

void BezierCurve::Paint(ArgParser *args)
{
    Curve::Paint(args);
    //DRAW THE CURVE
    glColor3f(0.f, 1.f, 0.f);
    glLineWidth(1);
    glBegin(GL_LINE_STRIP);
    float t = 0;
    float delta = 1.0f / args->curve_tessellation;
    Vec3f curve_pt;
    for (int c = 0; c < num_p - 3; c+=3)
    {
        t = 0;
        for (int i = 0; i <= args->curve_tessellation; i++)
        {
            curve_pt = GBT(c, t);
            glVertex3f(curve_pt[0], curve_pt[1], curve_pt[2]);
            t += delta;
        }
    }
    glEnd();
}

vector<Vec3f> BezierCurve::getPoints(ArgParser *args)
{
    float t = 0;
    float delta = 1.0f / args->curve_tessellation;
    vector<Vec3f> curve_pts;
    Vec3f curve_pt;
    for (int c = 0; c < num_p - 3; c += 3)
    {
        t = 0;
        for (int i = 0; i <= args->curve_tessellation; i++)
        {
            curve_pt = GBT(c, t);
            curve_pts.push_back(curve_pt);
            t += delta;
        }
    }
    return curve_pts;
}

void BSplineCurve::Paint(ArgParser *args)
{
    Curve::Paint(args);
    //DRAW THE CURVE
    glColor3f(0.f, 1.f, 0.f);
    glLineWidth(1);
    glBegin(GL_LINE_STRIP);
    float t = 0;
    float delta = 1.0f / args->curve_tessellation;
    Vec3f curve_pt;
    for (int c = 0; c < num_p - 3; c++)
    {
        t = 0;
        for (int i = 0; i <= args->curve_tessellation; i++)
        {
            curve_pt = GBT(c, t);
            glVertex3f(curve_pt[0], curve_pt[1], curve_pt[2]);
            t += delta;
        }
    }
    glEnd();
}

vector<Vec3f> BSplineCurve::getPoints(ArgParser *args)
{
    float t = 0;
    float delta = 1.0f / args->curve_tessellation;
    vector<Vec3f> curve_pts;
    Vec3f curve_pt;
    for (int c = 0; c < num_p - 3; c++)
    {
        t = 0;
        for (int i = 0; i <= args->curve_tessellation; i++)
        {
            curve_pt = GBT(c, t);
            curve_pts.push_back(curve_pt);
            t += delta;
        }
    }
    return curve_pts;
}

Vec3f Curve::GBT(int i, float t)
{
    Vec4f T(pow(t, 3), pow(t, 2), pow(t, 1), 1);
    B.Transform(T);
    return points[i] * T[0] + points[i + 1] * T[1] + points[i + 2] * T[2] + points[i + 3] * T[3];
}

Vec3f Curve::GBT(Vec3f p0, Vec3f p1, Vec3f p2, Vec3f p3, float t)
{
    Vec4f T(pow(t, 3), pow(t, 2), pow(t, 1), 1);
    B.Transform(T);
    return p0 * T[0] + p1 * T[1] + p2 * T[2] + p3 * T[3];
}

void BezierCurve::OutputBezier(FILE *file)
{
    fprintf(file, "%s", "bezier\nnum_vertices ");
    fprintf(file, "%d ", num_p);
    for (Vec3f pt : points)
    {
        fprintf(file, "%.1f %.1f %.1f ", pt[0], pt[1], pt[2]);
    }
}

void BezierCurve::OutputBSpline(FILE *file)
{
    BSplineCurve tmp(0);
    tmp.B.Inverse();
    Matrix trans = this->B * tmp.B;
    Vec3f pt;
    vector<Vec3f *> splines;
    for (int i = 0; i < num_p - 1; i += 3)
    {
        const float o_G[] = {
            points[i].x(), points[i + 1].x(), points[i + 2].x(), points[i + 3].x(),
            points[i].y(), points[i + 1].y(), points[i + 2].y(), points[i + 3].y(),
            points[i].z(), points[i + 1].z(), points[i + 2].z(), points[i + 3].z()};
        Matrix G = Matrix(o_G);
        Matrix result_matrix = G * trans;
        Vec3f vec_result[4] = {
            Vec3f(result_matrix.Get(0, 0), result_matrix.Get(0, 1), result_matrix.Get(0, 2)),
            Vec3f(result_matrix.Get(1, 0), result_matrix.Get(1, 1), result_matrix.Get(1, 2)),
            Vec3f(result_matrix.Get(2, 0), result_matrix.Get(2, 1), result_matrix.Get(2, 2)),
            Vec3f(result_matrix.Get(3, 0), result_matrix.Get(3, 1), result_matrix.Get(3, 2))};
        splines.push_back(vec_result);
    }
    for (Vec3f *spl : splines)
    {
        fprintf(file, "%s", "bspline num_vertices 4 ");
        fprintf(file, "%.1f %.1f %.1f ", spl[0].x(), spl[0].y(), spl[0].z());
        fprintf(file, "%.1f %.1f %.1f ", spl[1].x(), spl[1].y(), spl[1].z());
        fprintf(file, "%.1f %.1f %.1f ", spl[2].x(), spl[2].y(), spl[2].z());
        fprintf(file, "%.1f %.1f %.1f ", spl[3].x(), spl[3].y(), spl[3].z());
    }
}

void BSplineCurve::OutputBezier(FILE *file)
{
    BezierCurve tmp(0);
    tmp.B.Inverse();
    Matrix trans = this->B * tmp.B;
    Vec3f pt;
    vector<Vec3f *> splines;
    for (int i = 0; i < num_p - 1; i += 3)
    {
        const float o_G[] = {
            points[i].x(), points[i + 1].x(), points[i + 2].x(), points[i + 3].x(),
            points[i].y(), points[i + 1].y(), points[i + 2].y(), points[i + 3].y(),
            points[i].z(), points[i + 1].z(), points[i + 2].z(), points[i + 3].z()};
        Matrix G = Matrix(o_G);
        Matrix result_matrix = G * trans;
        Vec3f vec_result[4] = {
            Vec3f(result_matrix.Get(0, 0), result_matrix.Get(0, 1), result_matrix.Get(0, 2)),
            Vec3f(result_matrix.Get(1, 0), result_matrix.Get(1, 1), result_matrix.Get(1, 2)),
            Vec3f(result_matrix.Get(2, 0), result_matrix.Get(2, 1), result_matrix.Get(2, 2)),
            Vec3f(result_matrix.Get(3, 0), result_matrix.Get(3, 1), result_matrix.Get(3, 2))};
        splines.push_back(vec_result);
    }
    for (Vec3f *spl : splines)
    {
        fprintf(file, "%s", "bezier num_vertices 4 ");
        fprintf(file, "%.1f %.1f %.1f ", spl[0].x(), spl[0].y(), spl[0].z());
        fprintf(file, "%.1f %.1f %.1f ", spl[1].x(), spl[1].y(), spl[1].z());
        fprintf(file, "%.1f %.1f %.1f ", spl[2].x(), spl[2].y(), spl[2].z());
        fprintf(file, "%.1f %.1f %.1f ", spl[3].x(), spl[3].y(), spl[3].z());
    }
}

void BSplineCurve::OutputBSpline(FILE *file)
{
    fprintf(file, "%s", "bspline\nnum_vertices ");
    fprintf(file, "%d ", num_p);
    for (Vec3f pt : points)
    {
        fprintf(file, "%.1f %.1f %.1f ", pt[0], pt[1], pt[2]);
    }
}