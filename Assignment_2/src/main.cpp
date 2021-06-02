#include "hit.h"
#include "ray.h"
#include "image.h"
#include "material.h"
#include "matrix.h"
#include "string.h"
#include "camera.h"
#include "light.h"
#include "object3d.h"
#include "scene_parser.h"
#include <iostream>
#include <vector>

char *input_file = "scene2_03_colored_lights.txt";
int width = 200;
int height = 200;
char *output_file = "output2_03.tga";
float depth_min = 8;
float depth_max = 12;
char *depth_file = nullptr;
char *normals_file = "normals2_03.tga";
bool shade_back = false;

void prase_cmd(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-input"))
        {
            i++;
            assert(i < argc);
            input_file = argv[i];
        }
        else if (!strcmp(argv[i], "-size"))
        {
            i++;
            assert(i < argc);
            width = atoi(argv[i]);
            i++;
            assert(i < argc);
            height = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-output"))
        {
            i++;
            assert(i < argc);
            output_file = argv[i];
        }
        else if (!strcmp(argv[i], "-depth"))
        {
            i++;
            assert(i < argc);
            depth_min = atof(argv[i]);
            i++;
            assert(i < argc);
            depth_max = atof(argv[i]);
            i++;
            assert(i < argc);
            depth_file = argv[i];
        }
        else if (!strcmp(argv[i], "-normals"))
        {
            i++;
            assert(i < argc);
            normals_file = argv[i];
        }
        else if (!strcmp(argv[i], "-shade_back"))
        {
            shade_back = true;
        }
        else
        {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }
}

int main(int argc, char *argv[])
{
    prase_cmd(argc, argv);

    SceneParser sp(input_file);
    Vec3f groundColor = sp.getBackgroundColor();
    Camera *camera = sp.getCamera();
    vector<Material *> materials;
    vector<Light *> lights;
    int n_material = sp.getNumMaterials();
    int n_light = sp.getNumLights();

    for (int i = 0; i < n_material; i++)
    {
        materials.push_back(sp.getMaterial(i));
    }

    for (int i = 0; i < n_light; i++)
    {
        lights.push_back(sp.getLight(i));
    }

    Vec3f ambient = sp.getAmbientLight();

    float gray_scale = depth_max - depth_min;

    Group *group = sp.getGroup();
    Image img(width, height);
    Image depth_img(width, height);
    Image normal_img(width, height);

    img.SetAllPixels(groundColor);
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        {
            float fx = x / (float)width;
            float fy = y / (float)height;

            Ray r = camera->generateRay(Vec2f(fx, fy));
            Hit h = Hit(MAXFLOAT, materials[0], Vec3f(0, 0, 0));

            if (group->intersect(r, h, camera->getTMin()))
            {
                Vec3f pt = h.getIntersectionPoint();
                Vec3f pt_normal = h.getNormal();
                if (shade_back && pt_normal.Dot3(r.getDirection()) > 0)
                {
                    pt_normal = -1 * pt_normal;
                }
                Vec3f color = Vec3f(0, 0, 0);
                Vec3f dir2light;
                Vec3f diffM = h.getMaterial()->getDiffuseColor();

                color += ambient;

                for (int l = 0; l < n_light; l++)
                {
                    Vec3f tmp;
                    lights[l]->getIllumination(pt, dir2light, tmp);
                    color += tmp * max((pt_normal.Dot3(dir2light)), 0.0f);
                }

                color.Set(color.x() * diffM.x(), color.y() * diffM.y(), color.z() * diffM.z());

                img.SetPixel(x, y, color);

                float depth = h.getT();
                depth = max(depth, depth_min);
                depth = min(depth, depth_max);
                float gray = 1 - (depth - depth_min) / gray_scale;
                depth_img.SetPixel(x, y, Vec3f(gray, gray, gray));

                normal_img.SetPixel(x, y, h.getNormal());
            }
        }
    img.SaveTGA(output_file);
    if(depth_file)
        depth_img.SaveTGA(depth_file);
    if(normals_file)
        normal_img.SaveTGA(normals_file);
    return 0;
}