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

char *input_file = "test.txt";
int width = 300;
int height = 300;
char *output_file = "test.tga";
float depth_min = 8;
float depth_max = 12;
char *depth_file = nullptr;
char *normal_file = nullptr;
bool shade_back = false;
int theta_steps = 10;
int phi_steps = 10;
bool gui = true;
bool gouraud = false;
int bounces = 1;
float weight = 0.5;
bool shadows = false;
float epsilon = 0.1;
void render(){};

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
            normal_file = argv[i];
        }
        else if (!strcmp(argv[i], "-shade_back"))
        {
            shade_back = true;
        }
        else if (!strcmp(argv[i], "-gui"))
        {
            gui = true;
        }
        else if (!strcmp(argv[i], "-tessellation"))
        {
            i++;
            assert(i < argc);
            phi_steps = atoi(argv[i]);
            i++;
            assert(i < argc);
            theta_steps = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-gouraud"))
        {
            gouraud = true;
        }
        else if (!strcmp(argv[i], "-bounces"))
        {
            i++;
            assert(i < argc);
            bounces = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-weight"))
        {
            i++;
            assert(i < argc);
            weight = atof(argv[i]);
        }
        else if (!strcmp(argv[i], "-shadows"))
        {
            shadows = true;
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

    SceneParser* sp = new SceneParser(input_file);
    vector<Material *> materials;
    vector<Light *> lights;
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

    Camera *camera = sp->getCamera();
    Group *group = sp->getGroup();
    Vec3f background_color = sp->getBackgroundColor();
    Vec3f ambient_light = sp->getAmbientLight();

    Image *image = new Image(width, height);
    image->SetAllPixels(background_color);
    Image *depth_image = new Image(width, height);
    depth_image->SetAllPixels(Vec3f(0.0, 0.0, 0.0));
    Image *normal_image = new Image(width, height);
    normal_image->SetAllPixels(Vec3f(0.0, 0.0, 0.0));

    float precalc = depth_max - depth_min;
    if (precalc == 0)
    {
        precalc = 1.0f;
    }

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        {
            float fx = x / (float)width;
            float fy = y / (float)height;

            if (x == 150 && y == 250)
            {
                cout << "(x,y) = (150, 150)" << endl;
            }

            Ray r = camera->generateRay(Vec2f(fx, fy));
            Hit h = Hit(MAXFLOAT, materials[0], Vec3f(0, 0, 0));

            if (group->intersect(r, h, camera->getTMin()))
            {

                Vec3f pt = h.getIntersectionPoint();
                Vec3f pt_normal = h.getNormal();
                Vec3f color = Vec3f(0, 0, 0);
                Vec3f dir2light;
                float dist2light;
                for (int l = 0; l < n_light; l++)
                {
                    Vec3f light_color;
                    lights[l]->getIllumination(pt, dir2light, light_color, dist2light);
                    color += h.getMaterial()->Shade(r, h, dir2light, light_color);
                }
                color += h.getMaterial()->getDiffuseColor() * ambient_light;
                image->SetPixel(x, y, color);
                float depth = h.getT();
                depth = max(depth, depth_min);
                depth = min(depth, depth_max);
                float gray = 1 - (depth - depth_min) / precalc;
                depth_image->SetPixel(x, y, Vec3f(gray, gray, gray));
                normal_image->SetPixel(x, y, Vec3f(fabs(pt_normal.r()), fabs(pt_normal.g()), fabs(pt_normal.b())));
            }
        }

    if (output_file != NULL)
    {
        image->SaveTGA(output_file);
    }
    if (depth_file != NULL)
    {
        depth_image->SaveTGA(depth_file);
    }
    if (normal_file != NULL)
    {
        normal_image->SaveTGA(normal_file);
    }
    if(gui)
    {
        glutInit(&argc, argv);
        GLCanvas canvas;
        // canvas.initialize(sp, render);
    }
    
    return 0;
}