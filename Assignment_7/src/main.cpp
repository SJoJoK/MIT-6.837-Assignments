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
#include "rayTracer.h"
#include "sampler.h"
#include "film.h"
#include <iostream>
#include <vector>
char *input_file = (char *)"test.txt";
int width = 500;
int height = 500;
char *output_file = nullptr;
float depth_min = 8;
float depth_max = 12;
char *depth_file = nullptr;
char *normal_file = nullptr;
char *sample_file = nullptr;
bool shade_back = false;
int theta_steps = 20;
int phi_steps = 20;
bool gui = false;
bool gouraud = false;
int max_bounces = 1;
float cutoff_weight = 0.01;
bool shadows = false;
float epsilon = 0.01;
bool is_grid = true;
int nx = 15;
int ny = 30;
int nz = 15;
bool visualize_grid = false;
bool stats = false;
bool random_sample = false;
bool uniform_sample = false;
bool jittered_sample = false;
int sample_num = 1;
float zoom_factor = 1.0f;
SceneParser *sp;
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
        else if (!strcmp(argv[i], "-visualize_grid"))
        {
            visualize_grid = true;
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
            max_bounces = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-weight"))
        {
            i++;
            assert(i < argc);
            cutoff_weight = atof(argv[i]);
        }
        else if (!strcmp(argv[i], "-shadows"))
        {
            shadows = true;
        }
        else if (!strcmp(argv[i], "-stats"))
        {
            stats = true;
        }
        else if (!strcmp(argv[i], "-grid"))
        {
            is_grid = true;
            i++;
            assert(i < argc);
            nx = atoi(argv[i]);
            i++;
            assert(i < argc);
            ny = atoi(argv[i]);
            i++;
            assert(i < argc);
            nz = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-random_samples"))
        {
            random_sample = true;
            i++;
            assert(i < argc);
            sample_num = atof(argv[i]);
        }
        else if (!strcmp(argv[i], "-uniform_samples"))
        {
            uniform_sample = true;
            i++;
            assert(i < argc);
            sample_num = atof(argv[i]);
        }
        else if (!strcmp(argv[i], "-jittered_samples"))
        {
            jittered_sample = true;
            i++;
            assert(i < argc);
            sample_num = atof(argv[i]);
        }
        else if (!strcmp(argv[i], "-render_samples"))
        {
            i++;
            assert(i < argc);
            sample_file = argv[i];
            i++;
            assert(i < argc);
            zoom_factor = atof(argv[i]);
        }
        else
        {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }
}

void traceRayFunction(float x, float y)
{
    Ray r = sp->getCamera()->generateRay(Vec2f(x, y));
    Hit h(MAXFLOAT, nullptr, Vec3f(0.0f, 0.0f, 1.0f));
    RayTracer rt(sp);
    Vec3f pixel = rt.traceRay(r, epsilon, 0, 1.0, 1.0, h, true);
}

int main(int argc, char *argv[])
{
    prase_cmd(argc, argv);

    sp = new SceneParser(input_file);
    RayTracer *rt = new RayTracer(sp);
    Vec3f background_color = sp->getBackgroundColor();

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
    Sampler *spr;
    if (random_sample)
    {
        spr = new RandomSampler(sample_num);
    }
    else if (jittered_sample)
    {
        spr = new JitteredSampler(sample_num);
    }
    else
    {
        spr = new UniformSampler(sample_num);
    }
    Film film(width, height, sample_num);

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        {
            for (int n = 0; n < sample_num;n++)
            {
                Vec2f pixel_offset = spr->getSamplePosition(n);
                float fx = (x+pixel_offset.x()) / (float)width;
                float fy = (y+pixel_offset.y()) / (float)height;
                Ray r = sp->getCamera()->generateRay(Vec2f(fx, fy));
                r.x = x;
                r.y = y;
                Hit h = Hit(MAXFLOAT, nullptr, Vec3f(0, 0, 0));
                Vec3f color;
                color = rt->traceRay(r, epsilon, 0, 1, 1, h, true);
                Vec3f pt_normal = h.getNormal();
                float depth = h.getT();
                depth = max(depth, depth_min);
                depth = min(depth, depth_max);
                float gray = 1 - (depth - depth_min) / precalc;
                film.setSample(x, y, n, pixel_offset, color);
                // image->SetPixel(x, y, color);
                depth_image->SetPixel(x, y, Vec3f(gray, gray, gray));
                normal_image->SetPixel(x, y, Vec3f(fabs(pt_normal.x()), fabs(pt_normal.y()), fabs(pt_normal.z())));
            }
        }
    if(sample_file == nullptr)
    {
        image->SavePPM((char *)"test_sample.ppm");
    }
    else
    {
        film.renderSamples(sample_file, zoom_factor);
    }
    if (output_file == nullptr)
    {
        image->SavePPM((char *)"test.ppm");
    }
    else
    {
        image->SavePPM(output_file);
    }
    if (depth_file != nullptr)
    {
        depth_image->SavePPM(depth_file);
    }
    if (normal_file != nullptr)
    {
        normal_image->SavePPM(normal_file);
    }
    if (gui)
    {
        glutInit(&argc, argv);
        GLCanvas canvas;
        canvas.initialize(sp, render, traceRayFunction, rt->getGrid(), visualize_grid);
    }

    return 0;
}