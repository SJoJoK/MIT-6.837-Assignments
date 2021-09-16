#include "generator.h"
#include "GL/gl.h"
#include "GL/glu.h"
int HoseGenerator::numNewParticles(float current_time, float dt) const
{
    return dt * desired_num_particles / lifespan;
};
Particle* HoseGenerator::Generate(float current_time, int i)
{
    Vec3f p = this->position + this->random->randomVector() * this->positon_randomness;
    Vec3f v = this->velocity + this->random->randomVector() * this->velocity_randomness;
    float m = this->mass + this->random->next() * this->mass_randomness;
    float lf = this->lifespan + this->random->next() * this->mass_randomness;
    Particle *pt = new Particle(p, v, color, dead_color, m, lf);
    return pt;
}

int RingGenerator::numNewParticles(float current_time, float dt) const
{
    //最大两倍
    return dt * desired_num_particles * (current_time > 2.0 ? 2.0 : current_time) / lifespan;
};
Particle *RingGenerator::Generate(float current_time, int i)
{
    float radius = 3 * (current_time > 2.0 ? 2.0 : current_time) / 2.0;
    float theta = this->random->next() * 2 * M_PI;
    Vec3f p = Vec3f(radius * cosf(theta), 0, radius * sinf(theta)) + this->random->randomVector() * this->positon_randomness;
    Vec3f v = this->velocity + this->random->randomVector() * this->velocity_randomness;
    float m = this->mass + this->random->next() * this->mass_randomness;
    float lf = this->lifespan + this->random->next() * this->mass_randomness;
    Particle *pt = new Particle(p, v, color, dead_color, m, lf);
    return pt;
}
void RingGenerator::Paint() const
{
    glBegin(GL_QUADS);
    glVertex3f(-10.0, 0.0, 10.0);
    glVertex3f(10.0, 0.0, 10.0);
    glVertex3f(10.0, 0.0, -10.0);
    glVertex3f(-10.0, 0.0, -10.0);
    glEnd();
}