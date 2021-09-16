#include "surface.h"
TriangleMesh* SurfaceOfRevolution ::OutputTriangles(ArgParser *args)
{
    int ang_per_round = args->revolution_tessellation;
    float t = 0;
    float theta = 0;
    float delta_theta = 2 * M_PI / ang_per_round;
    Matrix rot_M;
    rot_M = rot_M.MakeYRotation(delta_theta);
    vector<Vec3f> o_pts = this->c->getPoints(args);
    vector<Vec3f> pts_1;
    TriangleNet *tn = new TriangleNet(o_pts.size()-1, ang_per_round);
    pts_1 = o_pts;
    for (int i = 0; i < ang_per_round + 1; i++)
    {
        for (int j = 0; j < o_pts.size(); j++)
        {
            tn->SetVertex(j, i, pts_1[j]);
        }
        for (Vec3f &pt : pts_1)
        {
            rot_M.Transform(pt);
        }
    }
    return tn;
}

TriangleMesh *BezierPatch ::OutputTriangles(ArgParser *args)
{
    float s = 0;
    float t = 0;
    float delta = 1.0f / args->patch_tessellation;
    TriangleNet *tn = new TriangleNet(args->patch_tessellation, args->patch_tessellation);
    Vec3f pt;
    for (int i = 0; i <= args->patch_tessellation; i++)
    {
        t = 0;
        for (int j = 0; j <= args->patch_tessellation; j++)
        {
            pt = this->c->GBT(
                this->c->GBT(this->points[0], this->points[1], this->points[2], this->points[3], t),
                this->c->GBT(this->points[4], this->points[5], this->points[6], this->points[7], t),
                this->c->GBT(this->points[8], this->points[9], this->points[10], this->points[11], t),
                this->c->GBT(this->points[12], this->points[13], this->points[14], this->points[15], t),
                s);
            t += delta;
            tn->SetVertex(i, j, pt);
        }
        s += delta;
    }
    return tn;
}