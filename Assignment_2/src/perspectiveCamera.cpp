#include "camera.h"
Ray PerspectiveCamera::generateRay(Vec2f point)
{
    const float dist = 1.0;
    Vec3f dy = 2 * tan(fov / 2.0) * dist * this->up;
    Vec3f dx = 2 * tan(fov / 2.0) * dist * ratio *this->horizontal;
    Vec3f pt = this->center + dist * this->direction - 0.5 * dx + point.x() * dx - 0.5 * dy + point.y() * dy;
    Vec3f dir = pt - this->center;
    return Ray(this->center, dir);
}
float PerspectiveCamera::getTMin() const
{
    return -1 * MAXFLOAT;
}