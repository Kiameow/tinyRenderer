#ifndef __SHADER_H__
#define __SHADER_H__

#include "geometry.h"
#include "tgaimage.h"
#include "global.h"

extern Matrix ModelAffine;
extern Matrix ViewAffine;  // transform from local coord. to camera coord. 
extern Matrix ProjectionAffine;
extern Matrix ViewPortAffine;  
extern Matrix Uniform_M;
extern Matrix Uniform_MIT;
extern Matrix ShadowAffineL;
extern Matrix ShadowAffine;

void viewport(int x, int y, int width, int height, int depth);
void projection(float coeff);
void lookat(Vec3f eye, Vec3f center, Vec3f up);
void uniform();
void model_affine(Vec3f rel_dest_pos, float rotation_deg, float scale);
void shadow();

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
    Vec4f vertex(int face_idx, int vert_idx) override {
        Vertex vertex = model->face(face_idx)[vert_idx];
        varying_intensity[vert_idx] = std::max(0.f, model->normal(vertex.normal_idx) * light_dir);
        varying_uv.set_col(vert_idx, model->uv(vertex.texture_idx));
        return ViewPortAffine * Uniform_M * embed<4>(model->vert(vertex.vertex_idx));
    }
    bool fragment(Vec3f bary, TGAColor &color) override {
        Vec2f uv = varying_uv * bary;
        float intensity = varying_intensity * bary;
        color = model->diffuse(uv) * intensity;
        //color = TGAColor(255, 255, 255, 255) * intensity;
        return false;
    }
};

class PhongShader : public IShader {
private:
    mat<2, 3, float> varying_uv;
    mat<3, 3, float> varying_vert;
public:
    ~PhongShader() {}
    Vec4f vertex(int face_idx, int vert_idx) override {
        Vertex vertex = model->face(face_idx)[vert_idx];
        varying_uv.set_col(vert_idx, model->uv(vertex.texture_idx));
        Vec4f gl_vert = ViewPortAffine * Uniform_M * embed<4>(model->vert(vertex.vertex_idx));
        varying_vert.set_col(vert_idx, proj<3>(gl_vert / gl_vert[3]));
        return gl_vert;
    }
    bool fragment(Vec3f bary, TGAColor &color) override {
        Matrix Uniform_Mshadow = ShadowAffine * (ViewPortAffine * Uniform_M).invert();
        Vec2f uv = varying_uv * bary;
        Vec4f shadow_vert = Uniform_Mshadow * embed<4>(varying_vert * bary);
        shadow_vert = shadow_vert / shadow_vert[3];
        float shadow = .3 + .7 * (shadow_buffer->get(shadow_vert[0], shadow_vert[1]) < shadow_vert[2] + 2);
        Vec3f n = proj<3>(Uniform_MIT * embed<4>(model->nm(uv))).normalize();
        Vec3f l = proj<3>(Uniform_M   * embed<4>(light_dir)).normalize();
        Vec3f r = (n*(n*l*2.f) - l).normalize();   // reflected light
        float spec = pow(std::max(r.z, 0.0f), model->spec(uv));
        float diff = std::max(0.f, n * l);        
        TGAColor c = model->diffuse(uv);
        for (int i=0; i<3; i++) {
            color[i] = std::min<float>(5 + c[i] * shadow * (diff + .6 * spec), 255);
        }
        //color = TGAColor(255, 255, 255, 255) * intensity;
        return false;
    }
};

class DepthShader : public IShader {
private:
    mat<3, 3, float> varying_vert;
public:
    ~DepthShader() {}
    Vec4f vertex(int face_idx, int vert_idx) override {
        Vertex vertex = model->face(face_idx)[vert_idx];
        Vec4f gl_vert = ViewPortAffine * Uniform_M * embed<4>(model->vert(vertex.vertex_idx));
        varying_vert.set_col(vert_idx, proj<3>(gl_vert / gl_vert[3]));
        return gl_vert;
    }
    bool fragment(Vec3f bary, TGAColor &color) override {
        Vec3f vert = varying_vert * bary;
        if (vert.z < 0) std::cout << vert.z << std::endl;
        color = TGAColor(255, 255, 255, 255) * (vert.z / 255);
        return false;
    }
};

#endif
