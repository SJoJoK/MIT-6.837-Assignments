#include "surface.h"
TriangleMesh* SurfaceOfRevolution ::OutputTriangles(ArgParser *args)
{
    //GET THE CURVE POINTS
    float t = 0;
    float delta = 1.0f / args->curve_tessellation;
    vector<Vec3f> o_pts;
    Vec3f curve_pt;
    this->c->num_p;
    for (int sp = 0; sp < this->c->num_p - 3; c += 3)
    {
        for (int i = 0; i <= args->curve_tessellation; i++)
        {
            curve_pt = this->c->GBT(sp, t);
            t += delta;
        }
    }
}