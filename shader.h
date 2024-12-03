#ifndef __SHADER_H__
#define __SHADER_H__

#include "geometry.h"
#include "tgaimage.h"
#include "global.h"

extern Matrix ModelView;
extern Matrix ViewPort;
extern Matrix Projection;
extern Matrix Uniform_M;
extern Matrix Uniform_MIT;

void viewport(int x, int y, int width, int height, int depth);
void projection(float coeff);
void lookat(Vec3f eye, Vec3f center, Vec3f up);
void uniform();
void rotate(float rotate_degree);

struct IShader {
    virtual ~IShader() {}
    virtual Vec4f vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bary, TGAColor &color) = 0;
};

class GouraudShader : public IShader {
private:
    Vec3f varying_intensity;
    mat<2, 3, float> varying_uv;
public:
    ~GouraudShader() {}
    Vec4f vertex(int face_idx, int vert_idx) override;
    bool fragment(Vec3f bary, TGAColor &color) override {
        Vec2f uv = varying_uv * bary;
        float intensity = varying_intensity * bary;
        color = model->diffuse(uv.x, uv.y) * intensity;
        //color = TGAColor(255, 255, 255, 255) * intensity;
        return false;
    }
};

class PhongShader : public IShader {
private:
    mat<3, 3, float> varying_normal;
    mat<2, 3, float> varying_uv;
public:
    ~PhongShader() {}
    Vec4f vertex(int face_idx, int vert_idx) override;
    bool fragment(Vec3f bary, TGAColor &color) override {
        Vec2f uv = varying_uv * bary;
        Vec3f normal = (varying_normal * bary);
        float intensity = std::max(0.f, normal * light_dir);
        
        color = model->diffuse(uv.x, uv.y) * intensity;
        //color = TGAColor(255, 255, 255, 255) * intensity;
        return false;
    }
};

class NormalBumpShader : public IShader {
private:
    mat<2, 3, float> varying_uv;
public:
    ~NormalBumpShader() {}
    Vec4f vertex(int face_idx, int vert_idx) override;
    bool fragment(Vec3f bary, TGAColor &color) override {
        Vec2f uv = varying_uv * bary;
        TGAColor normal_color = model->getNormal(uv.x, uv.y);
        Vec3f normal = Vec3f(normal_color.r, normal_color.g, normal_color.b).normalize();
        float intensity = std::max(0.f, normal * light_dir);
         
        color = model->diffuse(uv.x, uv.y) * intensity;
        //color = TGAColor(255, 255, 255, 255) * intensity;
        return false;
    }
};

#endif
