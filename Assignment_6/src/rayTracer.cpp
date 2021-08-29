#include "rayTracer.h"
#include "rayTree.h"
#include "object3d.h"
#include "camera.h"
#include "light.h"
RayTracer::RayTracer(SceneParser *s)
{
    this->sp = s;
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
    if (!is_grid)
    {
        grid = nullptr;
    }
    else
    {
        grid = new Grid(group->getBoundingBox(), nx, ny, nz);
        grid->material = group->material;
        group->insertIntoGrid(grid, nullptr);
    }
}
Vec3f RayTracer::mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const
{
    //out = incoming – 2 (incoming · normal) normal
    //里外是一致的
    Vec3f out = (incoming - normal * 2.0f * incoming.Dot3(normal));
    out.Normalize();
    return out;
}
bool RayTracer::transmittedDirection(const Vec3f &normal, const Vec3f &incoming, float index_i, float index_t, Vec3f &transmitted) const
{
    //ηr = ηi / ηt = index_i / index_t
    //I = -incoming
    //N = normal
    Vec3f n = normal;
    float NI = n.Dot3(incoming * -1.0f);

    if (index_t > 0.0f)
    {
        float index_r = index_i / index_t;
        float tmp = 1.0f - pow(index_r, 2.0f) * (1.0f - pow(NI, 2.0f));
        //非全反射
        if (tmp >= 0)
        {
            transmitted = n * (float)(index_r * NI - sqrt(tmp)) + incoming * index_r;
            transmitted.Normalize();
            return true;
        }
    }
    return false;
}
Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight,
                          float indexOfRefraction, Hit &hit, bool main, bool debug) const
{
    if (bounces > max_bounces)
        return Vec3f(0, 0, 0);
    if (weight < cutoff_weight)
        return Vec3f(0, 0, 0);
    if (group->intersect(ray, hit, epsilon))
    {
        if (main)
        {
            RayTree::SetMainSegment(ray, 0.0f, hit.getT());
        }
        Vec3f pt = hit.getIntersectionPoint();
        Vec3f pt_normal = hit.getNormal();
        pt_normal.Normalize();
        Vec3f color = hit.getMaterial()->getDiffuseColor() * ambient_light;
        Vec3f dir2light;
        float dist2light;
        //Shadow
        for (int l = 0; l < lights.size(); l++)
        {
            Vec3f light_color;
            lights[l]->getIllumination(pt, dir2light, light_color, dist2light);
            if (shadows)
            {
                Ray shadow_ray(pt, dir2light);
                Hit shadow_hit(dist2light, materials[0], Vec3f(0, 0, 0));
                Vec3f tmp;
                if (!group->intersectShadowRay(shadow_ray, shadow_hit, epsilon))
                {
                    tmp = hit.getMaterial()->Shade(ray, hit, dir2light, light_color);
                }
                if (debug && main)
                {
                    for (int i = 0; i < bounces; i++)
                    {
                        cout << " ";
                    }
                    cout << "Shadow: " << tmp << endl;
                }
                color += tmp;
                RayTree::AddShadowSegment(shadow_ray, 0.0f, shadow_hit.getT());
            }
            else
            {
                color += hit.getMaterial()->Shade(ray, hit, dir2light, light_color);
            }
        }
        //Mirror
        Vec3f mirror_color = hit.getMaterial()->getReflectiveColor();
        if (mirror_color != Vec3f(0, 0, 0))
        {
            Vec3f mirror_dir = mirrorDirection(pt_normal, ray.getDirection());
            Ray mirror_ray(pt, mirror_dir);
            Hit mirror_hit(dist2light, materials[0], Vec3f(0, 0, 0));
            Vec3f mirror_shade = traceRay(mirror_ray, epsilon, bounces + 1, weight * mirror_color.Length(),
                                          indexOfRefraction, mirror_hit, false, debug);
            if (debug && main)
            {
                for (int i = 0; i < bounces; i++)
                {
                    cout << " ";
                }
                cout << "Mirrow: " << mirror_color * mirror_shade << endl;
            }
            color += mirror_color * mirror_shade;
            RayTree::AddReflectedSegment(mirror_ray, 0.0f, mirror_hit.getT());
        }
        //Transparent
        Vec3f trans_color = hit.getMaterial()->getTransparentColor();
        if (trans_color != Vec3f(0, 0, 0))
        {
            Vec3f trans_dir;
            float index_t;
            float index_i;
            //Outside
            if (pt_normal.Dot3(ray.getDirection()) < 0)
            {
                index_i = 1.0f;
                index_t = hit.getMaterial()->getindexOfRefraction();
            }
            //Inside
            else
            {
                index_t = 1.0f;
                index_i = hit.getMaterial()->getindexOfRefraction();
                pt_normal.Negate();
            }
            //If transmitted
            if (transmittedDirection(pt_normal, ray.getDirection(), index_i, index_t, trans_dir))
            {
                Ray trans_ray(pt, trans_dir);
                Hit trans_hit(MAXFLOAT, materials[0], Vec3f(0, 0, 0));
                Vec3f trans_shade = traceRay(trans_ray, epsilon, bounces + 1, weight * trans_color.Length(), index_t, trans_hit, false, debug);
                if (debug && main)
                {
                    for (int i = 0; i < bounces; i++)
                    {
                        cout << " ";
                    }
                    cout << "Trans: " << trans_color * trans_shade << endl;
                }
                color += trans_color * trans_shade;
                RayTree::AddTransmittedSegment(trans_ray, 0.0f, trans_hit.getT());
            }
        }
        if (debug && main)
        {
            for (int i = 0; i < bounces; i++)
            {
                cout << " ";
            }
            cout << "Color: " << color << endl;
        }
        if ((color - Vec3f(0.4, 0.4, 0.32)).Length() <= 0.09 
            && ray.getDirection().y()>0
            )
        {
            cout << ray.getDirection() << endl;
            color = Vec3f(0, 0, 0);
            }
        return color;
    }
    return this->background_color;
}