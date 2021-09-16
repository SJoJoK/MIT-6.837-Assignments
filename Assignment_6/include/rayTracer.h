#include "scene_parser.h"
#include "ray.h"
#include "hit.h"
#include <vector>
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
extern int max_bounces;
extern float cutoff_weight;
extern bool shadows;
extern float epsilon;
extern bool visualize_grid;
extern int nx, ny, nz;
extern bool is_grid;
class RayTracer
{
private:
    SceneParser *sp;
    Grid *grid;
    Camera *camera;
    Group *group;
    Vec3f background_color;
    Vec3f ambient_light;
    vector<Material *> materials;
    vector<Light *> lights;

public:
    RayTracer(SceneParser *s);
    ~RayTracer(){};
    Grid *getGrid() { return grid; };
    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight,
                   float indexOfRefraction, Hit &hit, bool main, bool debug = false) const;
    Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const;
    bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming, float index_i, float index_t, Vec3f &transmitted) const;
};
