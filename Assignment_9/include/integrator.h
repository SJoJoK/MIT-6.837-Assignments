#pragma once
#include "forcefield.h"
#include "particle.h"
class Integrator
{
public:
    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) = 0;
};
class EulerIntegrator : Integrator
{
public:
    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
};

class MidpointIntegrator: Integrator
{
public:
    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
};