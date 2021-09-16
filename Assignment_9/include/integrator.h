#pragma once
#include "forcefield.h"
#include "particle.h"
class Integrator
{
public:
    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) = 0;
    virtual Vec3f getColor() = 0;
};
class EulerIntegrator : public Integrator
{
public:
    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
    virtual Vec3f getColor()
    {
        return Vec3f(0.5, 0.5, 0.2);
    }
};

class MidpointIntegrator: public Integrator
{
public:
    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
    virtual Vec3f getColor()
    {
        return Vec3f(0.5, 0.2, 0.5);
    }
};