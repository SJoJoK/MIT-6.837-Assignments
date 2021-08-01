#include "object3d.h"

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
    objs[i] = obj;
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin)
{
    // bool result = false;
    // MarchingInfo mi;
    // initializeRayMarch(mi, r, tmin);
    // if (mi.tmin < h.getT())
    // {
    //     while (mi.i < nx && mi.j < ny && mi.k < nz && mi.i >= 0 && mi.j >= 0 && mi.k >= 0)
    //     {
    //         if (m_is_voxel_opaque[(mi.i * ny + mi.j) * nz + mi.k])
    //         {
    //             if (objs[(mi.i * ny + mi.j) * nz + mi.k]->material == NULL)
    //                 objs[(mi.i * ny + mi.j) * nz + mi.k]->material = material;
    //             h.set(mi.tmin, objs[(mi.i * ny + mi.j) * nz + mi.k]->material, mi.normal, r);
    //             result = true;
    //             break;
    //         }
    //         mi.nextCell();
    //     }
    // }
    // return result;
}

void Grid::paint()
{
    material->glSetMaterial();
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