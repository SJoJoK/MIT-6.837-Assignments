#include "integrator.h"
void EulerIntegrator::Update(Particle *particle, ForceField *forcefield, float t, float dt)
{
    Vec3f p = particle->getPosition();
    Vec3f v = particle->getVelocity();
    Vec3f a = forcefield->getAcceleration(p, particle->getMass(), t);
    particle->setPosition(p + v * dt);
    particle->setVelocity(v + a * dt);
    particle->increaseAge(dt);
};
void MidpointIntegrator::Update(Particle *particle, ForceField *forcefield, float t, float dt)
{
    Vec3f p = particle->getPosition();
    Vec3f v = particle->getVelocity();
    Vec3f a = forcefield->getAcceleration(p, particle->getMass(), t);

    Vec3f pm = p + v * dt * 0.5;
    Vec3f vm = v + a * dt * 0.5;
    Vec3f am = forcefield->getAcceleration(pm, particle->getMass(), t + dt * 0.5);
    particle->setPosition(p + vm * dt);
    particle->setVelocity(v + am * dt);
    particle->increaseAge(dt);
};