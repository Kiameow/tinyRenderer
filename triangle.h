#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "global.h"
#include "geometry.h"
#include "tgaimage.h"
#include "zbuffer.h"
#include "shader.h"

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P);
void triangle(Vec4f *pts, GouraudShader shader, ZBuffer &zbuffer, TGAImage &image);
void triangle(Vec4f *pts, PhongShader shader, ZBuffer &zbuffer, TGAImage &image);

#endif
