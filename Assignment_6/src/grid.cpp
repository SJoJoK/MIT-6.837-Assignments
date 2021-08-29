#include "object3d.h"
extern bool visualize_grid;
extern float epsilon;
Vec3f Grid::getGird()
{
    return Vec3f(nx, ny, nz);
}

BoundingBox *Grid::getBoundingBox()
{
    return boundingBox;
}

void Grid::insertIntoThis(int i, bool v, Object3D *obj)
{
    m_is_voxel_opaque[i] = v;
    objs[i].push_back(obj);
}

void Grid::insertInfObj(Object3D *obj)
{
    inf_objs.push_back(obj);
}

void Grid::initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const
{
    const Vec3f D = r.getDirection();
    const Vec3f O = r.getOrigin();

    float _tnear = tmin;
    float _tfar = INFINITY;
    Vec3f _n(0.0f, 0.0f, 0.0f);

    assert(_tnear >= 0);

    // x axy
    float _dx = D.x();
    float _t1x = (boundingBox->getMin().x() - O.x()) / _dx;
    float _t2x = (boundingBox->getMax().x() - O.x()) / _dx;
    if (_t1x > _t2x)
    {
        float _tmp = _t1x;
        _t1x = _t2x;
        _t2x = _tmp;
    }

    // y axy
    float _dy = D.y();
    float _t1y = (boundingBox->getMin().y() - O.y()) / _dy;
    float _t2y = (boundingBox->getMax().y() - O.y()) / _dy;
    if (_t1y > _t2y)
    {
        float _tmp = _t1y;
        _t1y = _t2y;
        _t2y = _tmp;
    }

    // z axy
    float _dz = D.z();
    float _t1z = (boundingBox->getMin().z() - O.z()) / _dz;
    float _t2z = (boundingBox->getMax().z() - O.z()) / _dz;
    if (_t1z > _t2z)
    {
        float _tmp = _t1z;
        _t1z = _t2z;
        _t2z = _tmp;
    }

    _tnear = (_t1x > _tnear) ? _t1x : _tnear;
    _tnear = (_t1y > _tnear) ? _t1y : _tnear;
    _tnear = (_t1z > _tnear) ? _t1z : _tnear;

    _tfar = (_t2x < _tfar) ? _t2x : _tfar;
    _tfar = (_t2y < _tfar) ? _t2y : _tfar;
    _tfar = (_t2z < _tfar) ? _t2z : _tfar;

    if (_tfar < _tnear)
        return; // missed

    mi.sign_x = _dx > 0 ? 1 : -1;
    mi.sign_y = _dy > 0 ? 1 : -1;
    mi.sign_z = _dz > 0 ? 1 : -1;

    if (_tnear == _t1x)
        _n += Vec3f(-1.0f, 0.0f, 0.0f) * mi.sign_x;
    if (_tnear == _t1y)
        _n += Vec3f(0.0f, -1.0f, 0.0f) * mi.sign_y;
    if (_tnear == _t1z)
        _n += Vec3f(0.0f, 0.0f, -1.0f) * mi.sign_z;
    _n.Normalize();

    Vec3f min = boundingBox->getMin();
    Vec3f max = Vec3f(boundingBox->getMax().x(), boundingBox->getMax().y(), boundingBox->getMax().z());
    Vec3f size = max - min;
    float grid_x = size.x() / nx;
    float grid_y = size.y() / ny;
    float grid_z = size.z() / nz;

    mi.tmin = _tnear;
    mi.dt_x = fabs(grid_x / _dx);
    mi.dt_y = fabs(grid_y / _dy);
    mi.dt_z = fabs(grid_z / _dz);
    if (_t1x > -INFINITY)
    {
        while (_t1x <= _tnear)
        {
            _t1x += mi.dt_x;
        }
        mi.tnext_x = _t1x;
    }
    if (_t1y > -INFINITY)
    {
        while (_t1y <= _tnear)
        {
            _t1y += mi.dt_y;
        }
        mi.tnext_y = _t1y;
    }
    if (_t1z > -INFINITY)
    {
        while (_t1z <= _tnear)
        {
            _t1z += mi.dt_z;
        }
        mi.tnext_z = _t1z;
    }
    Vec3f _rp = O + D * _tnear - boundingBox->getMin();
    _rp.Divide(grid_x, grid_y, grid_z);
    mi.i = static_cast<int>(_rp.x());
    if (mi.sign_x < 0 && mi.i == nx)
        mi.i--;
    mi.j = static_cast<int>(_rp.y());
    if (mi.sign_y < 0 && mi.j == ny)
        mi.j--;
    mi.k = static_cast<int>(_rp.z());
    if (mi.sign_z < 0 && mi.k == nz)
        mi.k--;
    mi.normal = _n;
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin)
{
    // static int cnt = 0;
    bool result = false;
    MarchingInfo mi;
    initializeRayMarch(mi, r, tmin);
    Vec3f g_min = this->boundingBox->getMin();
    Vec3f g_max = this->boundingBox->getMax();
    Vec3f g_size = g_max - g_min;
    float grid_x = g_size.x() / this->nx;
    float grid_y = g_size.y() / this->ny;
    float grid_z = g_size.z() / this->nz;
    for (int i = 0; i < this->inf_objs.size(); i++)
    {
        result = inf_objs[i]->intersect(r, h, tmin) || result;
    }
    Hit pri_h(h);
    if (mi.tmin < h.getT())
    {
        if (visualize_grid)
        {
            while (mi.i < nx && mi.j < ny && mi.k < nz && mi.i >= 0 && mi.j >= 0 && mi.k >= 0)
            {
                int grid_id = (mi.i * ny + mi.j) * nz + mi.k;
                if (m_is_voxel_opaque[grid_id])
                {
                    if (objs[grid_id][0]->material == nullptr)
                        objs[grid_id][0]->material = new PhongMaterial(Vec3f(0.5, 0.5, 0.5));
                    Vec3f voxel_color(objs[grid_id].size() / 20.0f, 1 - objs[grid_id].size() / 20.0f, 0);
                    h.set(mi.tmin, new PhongMaterial(voxel_color), mi.normal, r);
                    result = true;
                    break;
                }
                mi.nextCell();
            }
        }
        else
        {
            while (mi.i < nx && mi.j < ny && mi.k < nz && mi.i >= 0 && mi.j >= 0 && mi.k >= 0)
            {
                int grid_id = (mi.i * ny + mi.j) * nz + mi.k;
                if (m_is_voxel_opaque[grid_id])
                {
                    for (int i = 0; i < objs[grid_id].size(); i++)
                    {
                        bool tmpres;
                        //try to intersect all the primitives in the cell
                        if (objs[grid_id][i]->pretrans_mat)
                        {
                            objs[grid_id][i] = new Transform(*(objs[grid_id][i]->pretrans_mat), objs[grid_id][i]);
                            tmpres = objs[grid_id][i]->intersect(r, h, tmin);
                        }
                        else
                            tmpres = objs[grid_id][i]->intersect(r, h, tmin);
                            // cnt++;
                        if (tmpres)
                        {
                            //get the intersection point
                            Vec3f p = h.getIntersectionPoint();
                            Vec3f rp = p - g_min;
                            //if the intersectionpoint is in the cell
                            if (rp.x() + epsilon >= mi.i * grid_x && rp.x() - epsilon <= (mi.i + 1) * grid_x &&
                                rp.y() + epsilon >= mi.j * grid_y && rp.y() - epsilon <= (mi.j + 1) * grid_y &&
                                rp.z() + epsilon >= mi.k * grid_z && rp.z() - epsilon <= (mi.k + 1) * grid_z)
                            {
                                return true;
                            }
                            //else, reset the hit
                            // cout << "The relatvie intersect point is " << rp << endl;
                            // cout << "But the Cell is " << endl
                            //      << "Min:" << Vec3f(mi.i * grid_x, mi.j * grid_y, mi.k * grid_z)
                            //      << " Max:" << Vec3f((mi.i+1) * grid_x, (1+mi.j) * grid_y, (1+mi.k) * grid_z) << endl;
                            h = pri_h;
                        }
                    }
                }
                mi.nextCell();
            }
        }
    }
    // cout << cnt << endl;
    return result;
}

void Grid::paint()
{
    // material->glSetMaterial();
    boundingBox->paint();
    Vec3f size = boundingBox->getMax() - boundingBox->getMin();
    float grid_x = size.x() / nx;
    float grid_y = size.y() / ny;
    float grid_z = size.z() / nz;
    Vec3f center = boundingBox->getMin();
    glPushMatrix();
    glTranslatef(center.x(), center.y(), center.z());
    for (int i = 0; i < nx * ny * nz; i++)
    {
        if (m_is_voxel_opaque[i])
        {
            int kk = i % nz;
            int jj = ((i - kk) / nz) % ny;
            int ii = (((i - kk) / nz) - jj) / ny;
            glBegin(GL_QUADS);
            glVertex3f(ii * grid_x, jj * grid_y, (kk + 1) * grid_z);
            glVertex3f((ii + 1) * grid_x, jj * grid_y, (kk + 1) * grid_z);
            glVertex3f((ii + 1) * grid_x, jj * grid_y, kk * grid_z);
            glVertex3f(ii * grid_x, jj * grid_y, kk * grid_z);

            glVertex3f(ii * grid_x, jj * grid_y, (kk + 1) * grid_z);
            glVertex3f(ii * grid_x, (jj + 1) * grid_y, (kk + 1) * grid_z);
            glVertex3f(ii * grid_x, (jj + 1) * grid_y, kk * grid_z);
            glVertex3f(ii * grid_x, jj * grid_y, kk * grid_z);

            glVertex3f(ii * grid_x, (jj + 1) * grid_y, (kk + 1) * grid_z);
            glVertex3f((ii + 1) * grid_x, (jj + 1) * grid_y, (kk + 1) * grid_z);
            glVertex3f((ii + 1) * grid_x, (jj + 1) * grid_y, kk * grid_z);
            glVertex3f(ii * grid_x, (jj + 1) * grid_y, kk * grid_z);

            glVertex3f((ii + 1) * grid_x, (jj + 1) * grid_y, kk * grid_z);
            glVertex3f((ii + 1) * grid_x, (jj + 1) * grid_y, (kk + 1) * grid_z);
            glVertex3f((ii + 1) * grid_x, jj * grid_y, (kk + 1) * grid_z);
            glVertex3f((ii + 1) * grid_x, jj * grid_y, kk * grid_z);

            glVertex3f(ii * grid_x, jj * grid_y, kk * grid_z);
            glVertex3f(ii * grid_x, (jj + 1) * grid_y, kk * grid_z);
            glVertex3f((ii + 1) * grid_x, (jj + 1) * grid_y, kk * grid_z);
            glVertex3f((ii + 1) * grid_x, jj * grid_y, kk * grid_z);

            glVertex3f(ii * grid_x, jj * grid_y, (kk + 1) * grid_z);
            glVertex3f((ii + 1) * grid_x, jj * grid_y, (kk + 1) * grid_z);
            glVertex3f((ii + 1) * grid_x, (jj + 1) * grid_y, (kk + 1) * grid_z);
            glVertex3f(ii * grid_x, (jj + 1) * grid_y, (kk + 1) * grid_z);
            glEnd();
        }
    }
    glPopMatrix();
}