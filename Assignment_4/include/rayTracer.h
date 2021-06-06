#include "scene_parser.h"
#include "ray.h"
#include "hit.h"
extern char *input_file;
extern int width;
extern int height;
extern char *output_file;
extern float depth_min;
extern float depth_max;
extern char *depth_file;
extern char *normal_file;
extern bool shade_back;
extern int theta_steps;
extern int phi_steps;
extern bool gui;
extern bool gouraud;
extern int bounces;
extern float weight;
extern bool shadows;

class RayTracer
{
private:
    SceneParser *sp;

public:
    RayTracer(SceneParser *s) : sp(s){};
    ~RayTracer();
    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight,
                   float indexOfRefraction, Hit &hit) const;
};
