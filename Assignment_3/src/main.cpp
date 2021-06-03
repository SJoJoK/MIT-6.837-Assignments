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

char *input_file = "scene2_16_t_scale.txt";
int width = 200;
int height = 200;
char *output_file = "output2_16.tga";
float depth_min = 8;
float depth_max = 12;
char *depth_file = "depth2_16.tga";
char *normals_file = "tmp.tga";
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

    SceneParser* sp = new SceneParser(input_file);
    GLCanvas canvas;
        // Vec3f groundColor = sp.getBackgroundColor();
        // Camera *camera = sp.getCamera();
        // vector<Material *> materials;
        // vector<Light *> lights;
        // int n_material = sp.getNumMaterials();
        // int n_light = sp.getNumLights();

        // for (int i = 0; i < n_material; i++)
        // {
        //     materials.push_back(sp.getMaterial(i));
        // }

        // for (int i = 0; i < n_light; i++)
        // {
        //     lights.push_back(sp.getLight(i));
        // }

        // Vec3f ambient = sp.getAmbientLight();

        // float gray_scale = depth_max - depth_min;

        // Group *group = sp.getGroup();
        return 0;
}