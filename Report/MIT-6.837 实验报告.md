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

在之前作业的基础上，使用OpenGL来预览自己的Ray Tracer

#### Assignment 4

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

#### Assignment 4

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

#### Assignment 4

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

Assignment 2基本实现了一个简单的Ray Tracer，但它还没有阴影、反射、折射和各种高级的着色方法，以及各种加速方法。我在做Assignment 2时遇到的最大的问题是我在Assignment 1中给自己留下的坑，这说明一个完善的项目打好基础是非常重要的，同时也要能学会怀疑自己的代码——并没有覆盖所有测试点的试例，所以过去的代码就算能跑过试例，也不代表就是正确的

#### Assignment 3

#### Assignment 4

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