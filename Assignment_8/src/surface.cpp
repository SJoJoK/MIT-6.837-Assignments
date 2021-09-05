#include "surface.h"
TriangleMesh* SurfaceOfRevolution ::OutputTriangles(ArgParser *args)
{
    int v_per_curve = args->curve_tessellation + 1;
    int ang_per_round = args->revolution_tessellation;
    int num_v = v_per_curve * ang_per_round;
    int num_f = 2 * (v_per_curve - 1) * ang_per_round;
    int cnt_v = 0;
    int cnt_f = 0;
    float t = 0;
    float theta = 0;
    float delta_curve = 1.0f / (v_per_curve-1);
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