#pragma once
#include "vectors.h"
#include "particle.h"
#include "random.h"
class Generator
{
public:
    Vec3f color;
    Vec3f dead_color;
    float color_randomness;

    float mass;
    float mass_randomness;

    float lifespan;
    float lifespan_randomness;

    int desired_num_particles;

    Random *random;
    Generator()
    {
        random = new Random(0);
    }
    virtual ~Generator()
    {
        delete random;
    }
    void SetColors(Vec3f color, Vec3f dead_color, float color_randomness)
    {
        this->color = color;
        this->dead_color = dead_color;
        this->color_randomness = color_randomness;
    }
    void SetLifespan(float lifespan, float lifespan_randomness, int desired_num_particles)
    {
        this->lifespan = lifespan;
        this->lifespan_randomness = lifespan_randomness;
        this->desired_num_particles = desired_num_particles;
    }
    void SetMass(float mass, float mass_randomness)
    {
        this->mass = mass;
        this->mass_randomness = mass_randomness;
    }

    // on each timestep, create some particles
    virtual int numNewParticles(float current_time, float dt) const {};
    virtual Particle *Generate(float current_time, int i){};

    // for the gui
    virtual void Paint() const {};
    virtual void Restart()
    {
        delete this->random;
        this->random = new Random(0);
    }
};

class HoseGenerator :public Generator
{
public:
    Vec3f position;
    Vec3f velocity;
    float positon_randomness;
    float velocity_randomness;
    HoseGenerator(Vec3f position, float position_randomness, Vec3f velocity, float velocity_randomness)
    {
        this->position = position;
        this->velocity = velocity;
        this->positon_randomness = position_randomness;
        this->velocity_randomness = velocity_randomness;
    }
    virtual int numNewParticles(float current_time, float dt) const;
    virtual Particle *Generate(float current_time, int i);
};

class RingGenerator : public Generator
{
public:
    Vec3f velocity;
    float positon_randomness;
    float velocity_randomness;
    RingGenerator(float position_randomness, Vec3f velocity, float velocity_randomness)
    {
        this->velocity = velocity;
        this->positon_randomness = position_randomness;
        this->velocity_randomness = velocity_randomness;
    }
    virtual int numNewParticles(float current_time, float dt) const;
    virtual Particle *Generate(float current_time, int i);

    // for the gui
    virtual void Paint() const;
    virtual void Restart();
};