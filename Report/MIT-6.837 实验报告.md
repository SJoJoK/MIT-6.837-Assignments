# MIT-6.837 实验报告

孙嘉锴，3180105871

## 目录

[TOC]

## Iterated Function Systems (Assigment 0)

### 实验要求

实现IFS分形系统

### 实验原理

![image-20210906203457568](MIT-6.837 实验报告.assets/image-20210906203457568.png)

![image-20210906203526949](MIT-6.837 实验报告.assets/image-20210906203526949.png)

### 源代码与分析

* IFS.h

```c++
#pragma once

#include "image.h"
#include "matrix.h"
#include "vectors.h"
#include <vector>

using namespace std;

class IFS
{
public:
    int n_trans;
    vector<Matrix> trans;
    vector<float> probs;
    vector<int> trans_cnts;
    Vec3f white = Vec3f(255, 255, 255);
    Vec3f black = Vec3f(0, 0, 0);
    IFS() : n_trans(0) {}
    void ifs_read(const char *filename);
    void ifs_render(Image &img, int n_pts, int n_iters);
};
```

* IFS.cpp

```cpp
#pragma once
#include <vector>
#include "image.h"
#include "matrix.h"
#include "IFS.h"

// ====================================================================
// ====================================================================
void IFS::ifs_read(const char* input_file)
{
    FILE *input = fopen(input_file, "r");
    assert(input != NULL);
    int num_transforms;
    fscanf(input, "%d", &num_transforms);
    n_trans = num_transforms;
    for (int i = 0; i < num_transforms; i++)
    {
        float probability;
        fscanf(input, "%f", &probability);
        Matrix m;
        m.Read3x3(input);
        probs.push_back(probability);
        trans.push_back(m);
    }
    fclose(input);
    for (int i = 1; i < n_trans; i++)
    {
        probs[i] += probs[i - 1];
    }
    for (int i = 0; i < n_trans; i++)
    {
        trans_cnts.push_back(0);
    }
}
void IFS::ifs_render(Image& img, int n_pts, int n_iters)
{
    img.SetAllPixels(white);
    for (int n = 0; n < n_pts; n++)
    {
        //For lots of random points...
        int x = rand() % img.Width();
        int y = rand() % img.Height();
        Vec2f pos = Vec2f(x, y);
        for (int k = 0; k < n_iters; k++)
        {
            //For numbers of iterations
            float sample = rand() / float(RAND_MAX);
            int i = 0;
            for (i = 0; i < n_trans; i++)
            {
                if (sample > probs[i])
                    continue;
                break;
            }
            //Pick a random transform ans use it
            if (i >= n_trans) i = n_trans - 1;
            Vec2f tmp(pos.x() / (float)img.Width(), pos.y() / img.Height());
            trans[i].Transform(tmp);
            pos.Set(tmp.x() * img.Width(), tmp.y() * img.Height());
            trans_cnts[i]++;
        }
        //display a dot
        if(img.isIn(pos.x(),pos.y()))
            img.SetPixel(pos.x(), pos.y(), black);
    }
    return;
}

// ====================================================================
// ====================================================================

```

IFS系统的实现比较简单，基本就是将伪代码翻译为C++代码，对于大学期间基本一直使用C++开发的我不算什么难事。我个人认为该代码中比较Tricky的一点是实现“按概率进行选取”，这里我在读取概率后进行了累加，相当于利用了概率分布函数而非概率密度函数

### 实验结果

**注**：由于我是在WSL(Windows Subsystem for Linux)上进行该系列实验的，使用VS Code进行连接，而VS Code目前只有.ppm而没有.tga的预览插件，所以输出.ppm文件会更方便一点。在发现这个插件之后，我就开始生成.ppm文件了。当然，在发现这个插件之前，我生成的还都是.tga文件。由于我是在把所有作业都做完后才开始写的报告，有的实验结果需要重新生成，而有些实验结果过去就生成过，所以实验结果既有可能是.tga文件也有可能是.ppm文件。若是.tga文件，我会使用截屏传统看图软件，若是.ppm文件，我会直接截屏VS Code上的预览，下同。

```zsh
./ifs -input sierpinski_triangle.txt -points 10000 -iters 30 -size 200 -output sierpinski_triangle.ppm
```

![image-20210906210313017](MIT-6.837 实验报告.assets/image-20210906210313017.png)

```zsh
./ifs -input fern.txt -points 50000 -iters 30 -size 400 -output fern.ppm
```

![image-20210906210353938](MIT-6.837 实验报告.assets/image-20210906210353938.png)

### 实验小结

我撰写报告时已据做完这个实验有3、4个月，因为从Assignment 0到Assignment 1之间我更换了电脑，而github上也只存储了源代码，所以又重新走了一遍构建、运行的流程。IFS本身设计的精妙不必多言，仅仅两重循环就可以构建一个自相似分形图像对于每一个初入图形学的同学来说都非常神奇，但我认为，它和L-System一样，都是“理论性”大于“实践性”的图形学方法，新奇性大于实用性，不过这个作业用来重温/熟悉C++以及引起同学对图形学的兴趣还是很有作用的。

## Ray Tracer (Assignment 1~7)

### 实验目的与要求

#### Assignment 1

实现一个非常基础的Ray Caster，只有正交相机，物体只有球体，也只有恒定的颜色，支持渲染深度图

#### Assignment 2

在Assignment 1的基础上，增加透视相机，增加平面、三角形、增加漫反射着色，支持渲染法向量图

#### Assignment 3

在之前作业的基础上，使用OpenGL来预览自己的Ray Caster，并增加Phong Shading

#### Assignment 4

在之前作业的基础上，增加阴影、反射与折射，在OpenGL预览中增加Ray Tree

#### Assignment 5

#### Assignment 6

#### Assignment 7

### 实验原理

#### Assignment 1

* 正交相机

  ![image-20210906213455071](MIT-6.837 实验报告.assets/image-20210906213455071.png)

* 球体求交

  * Algebraic

    ![image-20210906213746729](MIT-6.837 实验报告.assets/image-20210906213746729.png)

    ![image-20210906213802762](MIT-6.837 实验报告.assets/image-20210906213802762.png)

  * Geometric

    ![image-20210906213936009](MIT-6.837 实验报告.assets/image-20210906213936009.png)

#### Assignment 2

* 透视相机

  ![image-20210907152049335](MIT-6.837 实验报告.assets/image-20210907152049335.png)

* 平面求交

  ![image-20210907152927095](MIT-6.837 实验报告.assets/image-20210907152927095.png)

* 三角求交

  ![image-20210907153100088](MIT-6.837 实验报告.assets/image-20210907153100088.png)

  ![image-20210907153108976](MIT-6.837 实验报告.assets/image-20210907153108976.png)

  ![image-20210907153131167](MIT-6.837 实验报告.assets/image-20210907153131167.png)

  ![image-20210907153141311](MIT-6.837 实验报告.assets/image-20210907153141311.png)

* Transform

  对物体的变换

  ![image-20210907153620302](MIT-6.837 实验报告.assets/image-20210907153620302.png)

  ![image-20210907153634583](MIT-6.837 实验报告.assets/image-20210907153634583.png)

  ![image-20210907153702334](MIT-6.837 实验报告.assets/image-20210907153702334.png)

  对光线的变换

  ![image-20210907153458030](MIT-6.837 实验报告.assets/image-20210907153458030.png)

  ![image-20210907153518094](MIT-6.837 实验报告.assets/image-20210907153518094.png)

  对法向量的变换

  ![image-20210907153844901](MIT-6.837 实验报告.assets/image-20210907153844901.png)

  ![image-20210907153857189](MIT-6.837 实验报告.assets/image-20210907153857189.png)

* 漫反射着色

  ![image-20210907154355829](MIT-6.837 实验报告.assets/image-20210907154355829.png)

#### Assignment 3

* Phong Shading

  ![image-20210908172433807](C:\Users\45098\AppData\Roaming\Typora\typora-user-images\image-20210908172433807.png)

* Blinn-Torrance Variation

  ![1631093095669](C:\Users\45098\AppData\Local\Temp\utools-clipboard\1631093095669.png)

#### Assignment 4

* Shadow

  ![image-20210908220746016](C:\Users\45098\AppData\Roaming\Typora\typora-user-images\image-20210908220746016.png)

  ![image-20210908220905053](C:\Users\45098\AppData\Roaming\Typora\typora-user-images\image-20210908220905053.png)

* Mirror Reflection

  ![image-20210908220955086](C:\Users\45098\AppData\Roaming\Typora\typora-user-images\image-20210908220955086.png)

  ![image-20210908221009838](C:\Users\45098\AppData\Roaming\Typora\typora-user-images\image-20210908221009838.png)

* Refraction

  ![image-20210908221132110](C:\Users\45098\AppData\Roaming\Typora\typora-user-images\image-20210908221132110.png)

  ![image-20210908221144976](C:\Users\45098\AppData\Roaming\Typora\typora-user-images\image-20210908221144976.png)

* Ray Tracing

  ![image-20210908221254870](C:\Users\45098\AppData\Roaming\Typora\typora-user-images\image-20210908221254870.png)

#### Assignment 5

#### Assignment 6

#### Assignment 7

### 源代码与分析

#### Assignment 1

* camera.h

  ```c++
  #pragma once
  #include "matrix.h"
  #include "ray.h"
  
  class Camera
  {
  public:
      virtual Ray generateRay(Vec2f point) = 0;
      virtual float getTMin() const = 0;
      
  };
  ```

* orthographicCamera.h

  ```c++
  #pragma once
  #include "camera.h"
  class OrthographicCamera : public Camera
  {
  private:
      Vec3f center;
      Vec3f direction;
      Vec3f up;
      Vec3f horizontal;
      float size;
  
  public:
      OrthographicCamera(){};
      OrthographicCamera(Vec3f c, Vec3f dir, Vec3f up, double sz)
      {
          this->center = c;
          this->direction = dir;
          this->direction.Normalize();
          Vec3f::Cross3(this->horizontal, dir, up);
          this->horizontal.Normalize();
          Vec3f::Cross3(this->up, this->horizontal, this->direction);
          this->size = sz;
      }
      virtual Ray generateRay(Vec2f point);
      virtual float getTMin() const;
  };
  ```

* orthographicCamera.cpp

  ```c++
  #include "orthographicCamera.h"
  Ray OrthographicCamera::generateRay(Vec2f point)
  {
      Vec3f dx = size * this->horizontal;
      Vec3f dy = size * this->up;
      Vec3f pt = this->center - 0.5 * dx + point.x() * dx - 0.5 * dy + point.y() * dy;
      return Ray(pt, this->direction);
  }
  float OrthographicCamera::getTMin() const
  {
      return -1 * MAXFLOAT;
  }
  ```

  由于之前有过OpenGL/WebGL开发经验，所以实现相机还是比较容易的，遑论正交相机，只要在horizontal和up组成的平面上生成direction的光线即可

* object3d.h

  ```c++
  #pragma once
  #include "ray.h"
  #include "hit.h"
  #include "material.h"
  class Object3D
  {
  protected:
      Material *material;
  public:
      Object3D() : material(nullptr){};
      Object3D(Material *m) : material(m){};
      virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
      virtual ~Object3D(){};
  };
  ```

* group.h

  ```c++
  #pragma once
  #include "object3d.h"
  #include <vector>
  #include <array>
  class Group : public Object3D
  {
  private:
      int n_objs;
      Object3D **objs;
  
  public:
      Group() : n_objs(0){};
      Group(int n) : n_objs(n)
      {
          objs = new Object3D *[n];
      }
      void addObject(int index, Object3D *obj)
      {
          assert(index < n_objs);
          objs[index] = obj;
      }
      virtual bool intersect(const Ray &r, Hit &h, float tmin);
      ~Group()
      {
          for (int i = 0; i < n_objs; i++)
          {
              delete objs[i];
          }
          delete[] objs;
      }
  };
  
  ```

* sphere.h

  ```c++
  #pragma once
  #include "object3d.h"
  class Sphere : public Object3D
  {
  private:
      Vec3f center;
      double radius;
  
  public:
      Sphere() : center(Vec3f(0,0,0)), radius(0){};
      Sphere(Vec3f c, double r, Material *m) : Object3D(m), center(c), radius(r){};
      virtual bool intersect(const Ray &r, Hit &h, float tmin);
  };
  ```

* group.cpp

  ```c++
  #include "group.h"
  bool Group::intersect(const Ray &r, Hit &h, float tmin)
  {
      bool res = false;
      for (int i = 0; i < this->n_objs; i++)
      {
          if (objs[i] == nullptr)
              continue;
          res = res || objs[i]->intersect(r, h, tmin);
      }
      return res;
  }
  ```

* sphere.cpp

  ```c++
  #include "sphere.h"
  bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
  {
      Vec3f Rd = r.getDirection();
      Vec3f Ro = r.getOrigin() - this->center;
      // double a = 1;
      //从Assignment 2 回来， 这里坑实在太大了...千万别默认！
      double b = 2 * Rd.Dot3(Ro);
      double c = Ro.Dot3(Ro) - pow(this->radius, 2);
      double d_2 = b * b - 4 * c;
      if (d_2 < 0)
          return false;
      double d = sqrt(d_2);
      double t_p = (-1 * b + d) / 2;
      double t_n = (-1 * b - d) / 2;
      if (t_p < 0)
          return false;
      double tmp = h.getT();
      if (t_n < 0)
      {
          if (t_p < tmp && t_p > tmin)
          {
              h.set(t_p, this->material, r);
              return true;
          }
      }
      else
      {
          if (t_n < tmp && t_n > tmin)
          {
              h.set(t_n, this->material, r);
              return true;
          }
      }
      return false;
  }
  ```

  Group的intersect是调用Group中每一个Object的intersect方法（这里留下了一个很大的坑：我使用了```res = res || objs[i]->intersect(r, h, tmin);```，但根据C++的短路或，如果res为true，则后边的条件会被直接忽略，即不会继续与其他物体相交，我在后续的Assignment中才发现这个问题），而Sphere的相交算法我采取了比较符合直觉的代数方法，直接翻译方程为代码即可（此处默认a=1也为后续的Assignment留下了很大的坑）

* main.cpp

  ```c++
  for (int x = 0; x < width; x++)
          for (int y = 0; y < height; y++)
          {
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
  ```

  核心的渲染方程就是上述的二层循环，根据像素计算世界坐标系中的位置，再生成光线去检测相交即可，简单易懂

#### Assignment 2

* perspectiveCamera.cpp

  ```c++
  #include "camera.h"
  Ray PerspectiveCamera::generateRay(Vec2f point)
  {
      const float dist = 1.0;
      Vec3f dy = 2 * tan(fov / 2.0) * dist * this->up;
      Vec3f dx = 2 * tan(fov / 2.0) * dist * ratio *this->horizontal;
      Vec3f pt = this->center + dist * this->direction - 0.5 * dx + point.x() * dx - 0.5 * dy + point.y() * dy;
      Vec3f dir = pt - this->center;
      dir.Normalize();
      return Ray(this->center, dir);
  }
  float PerspectiveCamera::getTMin() const
  {
      return -1 * MAXFLOAT;
  }
  ```

  于之前有过OpenGL/WebGL开发经验，所以实现透视相机也比较容易，通过up、horizontal、center和三角函数计算射线上的另一个点并引出射线即可

* plane.cpp

  ```c++
  #include "object3d.h"
  bool Plane::intersect(const Ray &r, Hit &h, float tmin)
  {
      Vec3f Rd = r.getDirection();
      Vec3f Ro = r.getOrigin();
      float t = -1.0f * (distance + Ro.Dot3(this->normal)) / Rd.Dot3(this->normal);
      if (t > max(tmin,0.0f) && t < h.getT())
      {
          h.set(t, this->material, this->normal, r);
          return true;
      }
      return false;
  }
  ```

* triangle.cpp

  ```c++
  #include "object3d.h"
  bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
  {
      Vec3f e1 = b - a;
      Vec3f e2 = c - a;
      Vec3f s = r.getOrigin() - a;
      Vec3f d = r.getDirection();
      Vec3f s1;
      Vec3f::Cross3(s1, d, e2);
      Vec3f s2;
      Vec3f::Cross3(s2, s, e1);
      float det = e1.Dot3(s1);
      if (det == 0)
          return false;
      else
      {
          float t = s2.Dot3(e2) / det;
          float b1 = s1.Dot3(s) / det;
          float b2 = s2.Dot3(d) / det;
          if (t > max(0.0f, tmin) && b1 + b2 < 1 && b1 > 0 && b2 > 0 && t < h.getT())
          {
              h.set(t, this->material, this->normal, r);
              return true;
          }
      }
      return false;
  }
  ```

* transform.cpp

  ```c++
  #include "object3d.h"
  bool Transform::intersect(const Ray &r, Hit &h, float tmin)
  {
      bool res = false;
      Matrix tmp = this->transform_mat;
      if (tmp.Inverse())
      {
          Vec3f dir = r.getDirection();
          Vec3f o = r.getOrigin();
          tmp.Transform(o);
          tmp.TransformDirection(dir);
          //Without Normalization
          Ray nr(o, dir);
          res = obj->intersect(nr, h, tmin);
          if (res)
          {
              Vec3f n = h.getNormal();
              tmp.Transpose();
              tmp.TransformDirection(n);
              n.Normalize();
              h.set(h.getT(), h.getMaterial(), n, r);
          }
      }
      return res;
  }
  ```

  平面、三角形和变换的求交都是直接翻译课件上的算法，简单易懂

* main.cpp

  ```c++
  for (int x = 0; x < width; x++)
          for (int y = 0; y < height; y++)
          {
              float fx = x / (float)width;
              float fy = y / (float)height;
              // #if DEBUG
              if (x == 150 && y == 150)
              {
                  cout << "(x,y) = (100, 200)" << endl;
              }
              // #endif
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
  
                  normal_img.SetPixel(x, y, Vec3f(fabs(pt_normal.r()),fabs(pt_normal.g()),fabs(pt_normal.b())));
              }
          }
  ```

  在Assignment 1的基础上增加了漫反射着色与法向量绘制，漫反射着色使用

  ![image-20210907160630852](MIT-6.837 实验报告.assets/image-20210907160630852.png)

  方程进行计算，同时，当法向量方向与光线方向（从原摄像机开始）成锐角时，根据是否开启shadeback选择是否反转法向量

#### Assignment 3

* material.h

  ```c++
  class PhongMaterial : public Material
  {
  private:
      Vec3f specularColor;
      float exponent;
  
  public:
      PhongMaterial(const Vec3f &diffuseColor, const Vec3f &specularColor, float exponent) : Material(diffuseColor), specularColor(specularColor), exponent(exponent){};
      Vec3f getSpecularColor() const
      {
          return specularColor;
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
          if (shade_back && normal.Dot3(dirToCamera) > 0)
          {
              normal.Negate();
          }
          //Clamping
          float clamping = max(normal.Dot3(dirToLight), 0.0f);
          float distToLight = 1.0f;
          dirToCamera.Negate();
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
  ```
  
  实现Phong Shading的一个pitfall是BRDF中的Li与Lo都是从入射点出发的，而我们的Ray是从相机出发的，所以需要进行一下转换，此外，因为Blinn变种采用的是半角向量，所以当法向量和指向光的向量夹角为钝角时，高光也可能为正值，所以需要进行滤除
  
* Triangle.cpp

  ```c++
  void Triangle::paint()
  {
      material->glSetMaterial();
      glBegin(GL_TRIANGLES);
      glNormal3f(normal.x(), normal.y(), normal.z());
      glVertex3f(a.x(), a.y(), a.z());
      glVertex3f(b.x(), b.y(), b.z());
      glVertex3f(c.x(), c.y(), c.z());
      glEnd();
  }
  ```

* Plane.cpp

  ```c++
  void Plane::paint()
  {
      Vec3f d1, d2, v(1, 0, 0);
      if (normal.Dot3(v) == 0)
      {
          v = Vec3f(0, 1, 0);
      }
      Vec3f::Cross3(d1, v, normal);
      d1.Normalize();
      Vec3f::Cross3(d2, normal, d1);
      d2.Normalize();
      d1 *= 10000;
      d2 *= 10000;
      Vec3f e1 = d1 + d2, e2 = d1 * (-1) + d2, e3 = d1 * (-1) - d2, e4 = d1 - d2;
      e1 -= distance * normal;
      e2 -= distance * normal;
      e3 -= distance * normal;
      e4 -= distance * normal;
      material->glSetMaterial();
      glBegin(GL_QUADS);
      glNormal3f(normal.x(), normal.y(), normal.z());
      glVertex3f(e1.x(), e1.y(), e1.z());
      glVertex3f(e2.x(), e2.y(), e2.z());
      glVertex3f(e3.x(), e3.y(), e3.z());
      glVertex3f(e4.x(), e4.y(), e4.z());
      glEnd();
  }
  ```

  绘制平面即是绘制两个非常大的三角形，当然指定GL_QUADS也可以

* Sphere.cpp

  ```cpp
  void Sphere::paint()
  {
      //P(r, theta, phi)
      //theta 0-PI
      //phi 0-2PI
      float dtheta = PI / (float)theta_steps;
      float dphi = 2 * PI / (float)phi_steps;
      float theta = 0;
      glBegin(GL_QUADS);
      this->material->glSetMaterial();
      glPushMatrix();
      glTranslatef(center.x(), center.y(), center.z());
      for (int i = 0; i < theta_steps; i++)
      {
          float phi = 0;
          float theta_next = theta + dtheta;
          Vec3f pt0 = getSphereCoord(theta, phi);
          Vec3f pt1 = getSphereCoord(theta_next, phi);
          Vec3f pt2 = getSphereCoord(theta_next, phi + dphi);
          Vec3f pt3 = getSphereCoord(theta, phi + dphi);
          for (int j = 0; j < phi_steps ; j++)
          {
              if (j != 0)
              {
                  pt0 = pt3;
                  pt1 = pt2;
                  pt2 = getSphereCoord(theta_next, phi + dphi);
                  pt3 = getSphereCoord(theta, phi + dphi);
              }
              Vec3f normal;
              //Normals of Vertexs
              if (gouraud)
              {
                  normal = pt0 - this->center;
                  normal.Normalize();
                  glNormal3f(normal.x(), normal.y(), normal.z());
                  glVertex3f(pt0.x(), pt0.y(), pt0.z());
                  normal = pt1 - this->center;
                  normal.Normalize();
                  glNormal3f(normal.x(), normal.y(), normal.z());
                  glVertex3f(pt1.x(), pt1.y(), pt1.z());
                  normal = pt2 - this->center;
                  normal.Normalize();
                  glNormal3f(normal.x(), normal.y(), normal.z());
                  glVertex3f(pt2.x(), pt2.y(), pt2.z());
                  normal = pt3 - this->center;
                  normal.Normalize();
                  glNormal3f(normal.x(), normal.y(), normal.z());
                  glVertex3f(pt3.x(), pt3.y(), pt3.z());
              }
              else
              {
                  Vec3f a = pt3 - pt0;
                  Vec3f b = pt1 - pt0;
                  Vec3f::Cross3(normal, b, a);
                  normal.Normalize();
                  glNormal3f(normal.x(), normal.y(), normal.z());
                  glVertex3f(pt0.x(), pt0.y(), pt0.z());
                  glVertex3f(pt1.x(), pt1.y(), pt1.z());
                  glVertex3f(pt2.x(), pt2.y(), pt2.z());
                  glVertex3f(pt3.x(), pt3.y(), pt3.z());
              }
              phi += dphi;
          }
          theta += dtheta;
      }
      glPopMatrix();
      glEnd();
  ```

  绘制球体有两个需要注意的点：物理上和数学上对球座标的约定不一致，在我的实现中，$$\theta$$为天顶角，$$\phi$$为方位角，而MIT的作业中的约定与我的实现刚好相反，此外，需要注意要根据center的位置进行Transform

* Transform.cpp

  ```c++
  void Transform::paint()
  {
      glPushMatrix();
      GLfloat *glMatrix = this->transform_mat.glGet();
      glMultMatrixf(glMatrix);
      delete[] glMatrix;
      this->obj->paint();
      glPopMatrix();
  }
  ```

#### Assignment 4

* RayTracer.h

  ```c++
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
      ~RayTracer(){};
      Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight,
                     float indexOfRefraction, Hit &hit, bool main, bool debug=false) const;
      Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const;
      bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming, float index_i, float index_t, Vec3f &transmitted) const;
  };
  
  ```

* RayTracer.cpp

  ```c++
  #include "rayTracer.h"
  #include "rayTree.h"
  #include "object3d.h"
  #include "camera.h"
  #include "light.h"
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
                  color += trans_color * trans_shade;
                  RayTree::AddTransmittedSegment(trans_ray, 0.0f, trans_hit.getT());
              }
          }
          return color;
      }
      return this->background_color;
  }
  ```

  整个光线追踪里我个人认为最棘手的获取折射光线，要考虑内外介质的折射率、全反射、表面法向量种种问题，只有考虑清楚了才能写好。为了优化性能，还专门写了一个intersectShadowRay，只要有一次距离小于到光源的相交就return true

  此外，比较影响效果的是epsilon的选取，但它其实和整个光线追踪的流程没太大关系，但还是比较麻烦

#### Assignment 5

#### Assignment 6

#### Assignment 7

### 实验结果

#### Assignment 1

#### Assignment 2

#### Assignment 3

#### Assignment 4

#### Assignment 5

#### Assignment 6

#### Assignment 7

### 实验小结

#### Assignment 1

Assignment 1是整个Ray Tracer的基础，虽然简单但非常重要，非常有利于理解光线追踪的本质。我初学MIT-6.837时，还认为作业会是使用OpenGL等图形API做一些图形流水线方面的工作，但果然基于物理和直觉的的Ray Tracer才是图形学的精粹。第一次使用纯C++而没有使用任何图形接口实现3D模型的渲染，其成就感也非同小可

#### Assignment 2

Assignment 2基本实现了一个简单的Caster，但它还没有阴影、反射、折射和各种高级的着色方法，以及各种加速方法。我在做Assignment 2时遇到的最大的问题是我在Assignment 1中给自己留下的坑，这说明一个完善的项目打好基础是非常重要的，同时也要能学会怀疑自己的代码——并没有覆盖所有测试点的试例，所以过去的代码就算能跑过试例，也不代表就是正确的

#### Assignment 3

Assignment 3最重要的就是实现了Phong Shading，拥有高光之后，我们的渲染就更加真实了。OpenGL的实现更像是一个填头，一方面可以让我们大概看出应该实现效果的“Ground Truth”，另一方面可以让我们领略到图形流水线的威力——CPU写的软光追还需要一定时间才能渲染出来的图，OpenGL可以实时浏览

#### Assignment 4

Assignment 4直接把我们的Ray Caster升格为Ray Tracer，我们的项目不再是一个光线投射器，而是一个可以（反向）追踪光线的光线追踪器，加入了阴影、反射和折射后的渲染质量也更上一个台阶。我在这个作业中遇到的坑主要有两个，一个是在处理阴影时，将向光源射线的循环放在外边了，导致反射和折射都被计算了（num_lights）次，后来靠自己添加调试信息才发现这个问题，另一个是epsilon的选取，太小的话会有自阴影等问题，但太大的话算法的准确性就难以保证，是一个很tricky的问题。总得来说，加入了阴影、反射和折射后，就是实现了一个暂时没有任何优化的软光追，还是很有成就感的。

#### Assignment 5

#### Assignment 6

#### Assignment 7

## Curve Editor (Assignment 8)

### 实验目的与要求

### 实验原理

### 源代码与分析

### 实验小结

## Particle System (Assignment 9)

### 实验目的与要求

### 实验原理

### 源代码与分析

### 实验小结