#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "glCanvas.h"
#include <GL/gl.h>
#include <GL/glut.h>
#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

#include "matrix.h"
#include "vectors.h"
#include "hit.h"
#include "perlin_noise.h"
extern bool shade_back;
// ====================================================================
// ====================================================================

// You will extend this class in later assignments

class Material
{

public:
    // CONSTRUCTORS & DESTRUCTOR
    Material(const Vec3f &d_color) { diffuseColor = d_color; }
    virtual ~Material() {}
    // ACCESSORS
    virtual Vec3f getDiffuseColor() const
    {
        return diffuseColor;
    }
    virtual Vec3f getSpecularColor() const
    {
        return specularColor;
    }
    virtual Vec3f getReflectiveColor() const
    {
        return reflectiveColor;
    }
    virtual Vec3f getTransparentColor() const
    {
        return transparentColor;
    }
    virtual float getindexOfRefraction() const
    {
        return indexOfRefraction;
    }

    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const = 0;
    virtual void glSetMaterial(void) const = 0;

protected:
    // REPRESENTATION
    Vec3f diffuseColor = Vec3f(1,1,1);
    Vec3f specularColor = Vec3f(0,0,0);
    Vec3f reflectiveColor = Vec3f(0,0,0);
    Vec3f transparentColor = Vec3f(0,0,0);
    float indexOfRefraction = 1;
};

// ====================================================================
// ====================================================================

class PhongMaterial : public Material
{
private:
    float exponent=1;

public:
    PhongMaterial(const Vec3f &diffuseColor) : Material(diffuseColor)
                  {};
    PhongMaterial(const Vec3f &diffuseColor,
                  const Vec3f &specularColor,
                  float exponent,
                  const Vec3f &reflectiveColor,
                  const Vec3f &transparentColor,
                  float indexOfRefraction) : Material(diffuseColor)
    {
        this->specularColor = specularColor;
        this->reflectiveColor = reflectiveColor;
        this->transparentColor = transparentColor;
        this->exponent = exponent;
        this->indexOfRefraction = indexOfRefraction;
    }
    virtual void glSetMaterial(void) const
    {

        GLfloat one[4] = {1.0, 1.0, 1.0, 1.0};
        GLfloat zero[4] = {0.0, 0.0, 0.0, 0.0};
        GLfloat specular[4] = {
            getSpecularColor().r(),
            getSpecularColor().g(),
            getSpecularColor().b(),
            1.0};
        GLfloat diffuse[4] = {
            getDiffuseColor().r(),
            getDiffuseColor().g(),
            getDiffuseColor().b(),
            1.0};

        // NOTE: GL uses the Blinn Torrance version of Phong...
        float glexponent = exponent;
        if (glexponent < 0)
            glexponent = 0;
        if (glexponent > 128)
            glexponent = 128;

#if !SPECULAR_FIX

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

        // OPTIONAL: 3 pass rendering to fix the specular highlight
        // artifact for small specular exponents (wide specular lobe)

        if (SPECULAR_FIX_WHICH_PASS == 0)
        {
            // First pass, draw only the specular highlights
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);
        }
        else if (SPECULAR_FIX_WHICH_PASS == 1)
        {
            // Second pass, compute normal dot light
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
        }
        else
        {
            // Third pass, add ambient & diffuse terms
            assert(SPECULAR_FIX_WHICH_PASS == 2);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
        }

#endif
    }
    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
    {
        Vec3f normal = hit.getNormal();
        //Ray is Camera to Point
        Vec3f dirToCamera = ray.getDirection();
        dirToCamera.Negate();
        //Clamping
        float clamping = max(normal.Dot3(dirToLight), 0.0f);
        if (shade_back && normal.Dot3(dirToCamera) < 0)
        {
            normal.Negate();
        }
        float distToLight = 1.0f;
        Vec3f half = dirToCamera + dirToLight;
        half.Normalize();
        float r = 1.0f;
        float diffuse = max(dirToLight.Dot3(normal), 0.0f);
        float specular = max(normal.Dot3(half), 0.0f);
        float shiness = pow(specular, exponent);
        Vec3f color = diffuseColor * diffuse + specularColor * shiness;
        color = color * lightColor * (1 / pow(distToLight, 2));
        color = color * clamping;
        return color;
    }
};

class Checkerboard : public Material
{
public:
    Matrix *mat;
    Material *material1, *material2;
    Checkerboard(Matrix *m, Material *mat1, Material *mat2) : Material(Vec3f(1,1,1)), mat(m), material1(mat1), material2(mat2){};
    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
    {
        Vec3f p = hit.getIntersectionPoint();
        mat->Transform(p);
        bool whichmaterial = (int(floor(p[0])) + int(floor(p[1])) + int(floor(p[2]))) % 2;
        if (whichmaterial)
        {
            return material1->Shade(ray, hit, dirToLight, lightColor);
        }
        else
        {
            return material2->Shade(ray, hit, dirToLight, lightColor);
        }
        return Vec3f();
    }
    void glSetMaterial(void) const
    {
        material1->glSetMaterial();
    }
};

class Noise : public Material
{
public:
    int octaves;
    Matrix *mat;
    Material *material1, *material2;
    static float NoiseCalculate(Vec3f pos, int octaves)
    {
        float c = 0;
        float iteration = 1.0;
        for (int i = 0; i < octaves; i++)
        {
            c += PerlinNoise::noise(pos.x(), pos.y(), pos.z()) * iteration;
            iteration /= 2.0;
            pos *= 2.0f;
        }
        return c;
    }
    Noise(Matrix *m, Material *mat1, Material *mat2, int _octaves) : Material(Vec3f(1,1,1)), mat(m), material1(mat1), material2(mat2), octaves(_octaves){};
    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
    {
        Vec3f p = hit.getIntersectionPoint();
        mat->Transform(p);
        float c = Noise::NoiseCalculate(p, octaves);
        Vec3f color1 = material1->Shade(ray, hit, dirToLight, lightColor);
        Vec3f color2 = material2->Shade(ray, hit, dirToLight, lightColor);
        return color1 * (1 - c) + color2 * c;
    }
    void glSetMaterial(void) const
    {
        material1->glSetMaterial();
    }
};

class Marble : public Material
{
public:
    int octaves;
    Matrix *mat;
    Material *material1, *material2;
    float frequency;
    float amplitude;
    Marble(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude)
        : Material(Vec3f(1,1,1)), mat(m), material1(mat1), material2(mat2), octaves(octaves), frequency(frequency), amplitude(amplitude){};
    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
    {
        Vec3f p = hit.getIntersectionPoint();
        mat->Transform(p);
        float c = sin(frequency * p.x() + amplitude * Noise::NoiseCalculate(p, octaves));
        Vec3f color1 = material1->Shade(ray, hit, dirToLight, lightColor);
        Vec3f color2 = material2->Shade(ray, hit, dirToLight, lightColor);
        return color1 * (1 - c) + color2 * c;
    }
    void glSetMaterial(void) const
    {
        material1->glSetMaterial();
    }
};

class Wood : public Material
{
public:
    Wood(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude) : Material(Vec3f(1,1,1)){};
    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
    {
        return Vec3f(1, 1, 1);
    }
    void glSetMaterial(void) const
    {
        return;
    }
};
#endif