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
class RayTracer
{
private:
    SceneParser *sp;
    Camera *camera;
    Group *group;
    Vec3f background_color;
    Vec3f ambient_light;
    vector<Material *> materials;
    vector<Light *> lights;

public:
    RayTracer(SceneParser *s) : sp(s)
    {
        camera = sp->getCamera();
        group = sp->getGroup();
        background_color = sp->getBackgroundColor();
        ambient_light = sp->getAmbientLight();
        int n_material = sp->getNumMaterials();
        int n_light = sp->getNumLights();
        for (int i = 0; i < n_material; i++)
        {
            materials.push_back(sp->getMaterial(i));
        }

        for (int i = 0; i < n_light; i++)
        {
            lights.push_back(sp->getLight(i));
        }
    }
    ~RayTracer();
    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight,
                   float indexOfRefraction, Hit &hit, bool main) const;
    Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const;
    bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming, float index_i, float index_t, Vec3f &transmitted) const;
};
