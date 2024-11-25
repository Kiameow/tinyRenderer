#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "geometry.h"
#include "tgaimage.h"
#include "zbuffer.h"

#define EPSILON 0.001f

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P);
void triangle(Vec3f *pts, ZBuffer& zbuffer, TGAImage &image, TGAColor* colors);
void triangle(Vec3f *pts, ZBuffer& zbuffer, TGAImage &image, Vec3f *texts, TGAImage &tetxure, float intensity, Vec3f persp);

#endif
