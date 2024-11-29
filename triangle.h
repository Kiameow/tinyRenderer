#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "global.h"
#include "geometry.h"
#include "tgaimage.h"
#include "zbuffer.h"

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P);
void triangle(Vec3f *pts, Vec2i *texts, Vec3f *normals, ZBuffer &zbuffer, TGAImage &image);

#endif
