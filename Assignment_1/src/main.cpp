#include "group.h"
#include "hit.h"
#include "ray.h"
#include "image.h"
#include "material.h"
#include "matrix.h"
#include "string.h"
#include "scene_parser.h"
#include "orthographicCamera.h"
#include <iostream>
#include <vector>
int main(int argc, char *argv[])
{

    char *input_file = "scene1_06.txt";
    int width = 200;
    int height = 200;
    char *output_file = "output1_06.tga";
    float depth_min = 8;
    float depth_max = 12;
    char *depth_file = "depth1_06.tga";

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
        else
        {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }
    float gray_scale = depth_max - depth_min;
    SceneParser sp(input_file);
    Vec3f groundColor = sp.getBackgroundColor();
    Camera *camera = sp.getCamera();
    int n_material = sp.getNumMaterials();
    vector<Material *> materials;
    for (int i = 0; i < n_material; i++)
    {
        materials.push_back(sp.getMaterial(i));
    }
    Group *group = sp.getGroup();
    Image img(width, height);
    img.SetAllPixels(groundColor);
    Image depth_img(width, height);
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        {
            if(x==110&&y==100)
            {
                cout << "F";
            }
            float fx = x / (float)width;
            float fy = y / (float)height;
            Ray r = camera->generateRay(Vec2f(fx, fy));
            Hit h = Hit(MAXFLOAT, materials[0]);
            if (group->intersect(r, h, camera->getTMin()))
            {
                img.SetPixel(x, y, h.getMaterial()->getDiffuseColor());
                float depth = h.getT();
                depth = max(depth, depth_min);
                depth = min(depth, depth_max);
                float gray = 1 - (depth - depth_min) / gray_scale;
                depth_img.SetPixel(x, y, Vec3f(gray, gray, gray));
            }
        }
    img.SaveTGA(output_file);
    depth_img.SaveTGA(depth_file);
    return 0;
}