#include "camera.h"
Ray PerspectiveCamera::generateRay(Vec2f point)
{
    const float dist = 1.0;
    Vec3f dy = 2 * tan(fov / 2.0) * dist * this->up;
    Vec3f dx = 2 * tan(fov / 2.0) * dist * ratio *this->horizontal;
    Vec3f pt = this->center + dist * this->direction - 0.5 * dx + point.x() * dx - 0.5 * dy + point.y() * dy;
    Vec3f dir = pt - this->center;
    dir.Normalize();
    return Ray(this->center, dir);
}
float PerspectiveCamera::getTMin() const
{
    return -1 * MAXFLOAT;
}
// ====================================================================
// Create a perspective camera with the appropriate dimensions that
// crops or stretches in the x-dimension as necessary
// ====================================================================

void PerspectiveCamera::glInit(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov * 180.0 / 3.14159, (float)w / float(h), 0.5, 40.0);
}

// ====================================================================
// Place a perspective camera within an OpenGL scene
// ====================================================================

void PerspectiveCamera::glPlaceCamera(void)
{
    gluLookAt(center.x(), center.y(), center.z(),
              center.x() + direction.x(), center.y() + direction.y(), center.z() + direction.z(),
              up.x(), up.y(), up.z());
}

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void PerspectiveCamera::dollyCamera(float dist)
{
    center += direction * dist;

    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================
}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void PerspectiveCamera::truckCamera(float dx, float dy)
{
    Vec3f horizontal;
    Vec3f::Cross3(horizontal, direction, up);
    horizontal.Normalize();

    Vec3f screenUp;
    Vec3f::Cross3(screenUp, horizontal, direction);

    center += horizontal * dx + screenUp * dy;

    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================
}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void PerspectiveCamera::rotateCamera(float rx, float ry)
{
    Vec3f horizontal;
    Vec3f::Cross3(horizontal, direction, up);
    horizontal.Normalize();

    // Don't let the model flip upside-down (There is a singularity
    // at the poles when 'up' and 'direction' are aligned)
    float tiltAngle = acos(up.Dot3(direction));
    if (tiltAngle - ry > 3.13)
        ry = tiltAngle - 3.13;
    else if (tiltAngle - ry < 0.01)
        ry = tiltAngle - 0.01;

    Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
    rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

    rotMat.Transform(center);
    rotMat.TransformDirection(direction);
    direction.Normalize();
}

// ====================================================================
// ====================================================================