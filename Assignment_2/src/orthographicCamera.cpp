#include "camera.h"
Ray OrthographicCamera::generateRay(Vec2f point)
{
    Vec3f dx = size * this->horizontal;
    Vec3f dy = size * this->up;
    Vec3f pt = this->center - 0.5 * dx + point.x() * dx - 0.5 * dy + point.y() * dy;
    return Ray(pt, this->direction);
}
float OrthographicCamera::getTMin() const
{
    return -1 * MAXFLOAT;
}