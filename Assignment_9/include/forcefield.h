#include "vectors.h"
class ForceField
{
public:
    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const = 0;
};
class GravityForceField : public ForceField
{
public:
    Vec3f g;
    GravityForceField(Vec3f gravity)
    {
        this->g = gravity;
    };
    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t)
    {
        return g;
    }
};

class ConstantForceField : public ForceField
{
public:
    Vec3f f;
    ConstantForceField(Vec3f force)
    {
        this->f = force;
    };
    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t)
    {
        return f * (1/mass);
    }
};
class RadialForceField : public ForceField
{
public:
    float mag;
    RadialForceField(float magnitude)
    {
        this->mag = magnitude;
    };
    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t)
    {
        return mag * (Vec3f(0, 0, 0) - position);
    }
};
class VerticalForceField : public ForceField
{
public:
    float mag;
    VerticalForceField(float magnitude)
    {
        this->mag = magnitude;
    };
    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t)
    {
        return mag * (Vec3f(position.x(), 0, position.z()));
    }
};