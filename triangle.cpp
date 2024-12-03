#include "triangle.h"
#include "global.h"
#include <iostream>

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
    Vec3f c1 = Vec3f(B.x - A.x, C.x - A.x, A.x - P.x);
    Vec3f c2 = Vec3f(B.y - A.y, C.y - A.y, A.y - P.y);

    Vec3f u = cross(c1, c2);
    if (std::abs(u.z) < EPSILON) {
        // invalid output
        return Vec3f(-1, 0, 0);
    }
    Vec3f un = Vec3f(u.x / u.z, u.y / u.z, 1);
    if (un.x < 0 || un.y < 0 || un.x + un.y > 1)  return Vec3f(-1, 0, 0);
    return Vec3f(1 - (un.x + un.y), un.x, un.y);
}

void triangle(Vec4f *pts, GouraudShader shader, ZBuffer &zbuffer, TGAImage &image) {
    Vec2f bboxmin(image.get_width() - 1, image.get_height() - 1);
    Vec2f bboxmax(0, 0);
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++) {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    
    for (int y = bboxmin.y; y <= bboxmax.y; y++) {
        for (int x = bboxmin.x; x <= bboxmax.x; x++) {
            Vec3f bary = barycentric(proj<2>(pts[0]), proj<2>(pts[1]), proj<2>(pts[2]), Vec2f(x, y));
            if (bary.x < 0 || bary.y < 0 || bary.z < 0) continue;

            Vec3f zs(pts[0][2], pts[1][2], pts[2][2]);
            float inter_z = zs * bary;

            if (inter_z < zbuffer.get(x, y)) continue;

            TGAColor c;
            bool discard = shader.fragment(bary, c);

            if (discard) c = TGAColor(0, 0, 0, 255);
     
            zbuffer.set(x, y, inter_z);
            image.set(x, y, c);
        }
    }
}

void triangle(Vec4f *pts, PhongShader shader, ZBuffer &zbuffer, TGAImage &image) {
    Vec2f bboxmin(image.get_width() - 1, image.get_height() - 1);
    Vec2f bboxmax(0, 0);
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++) {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    
    for (int y = bboxmin.y; y <= bboxmax.y; y++) {
        for (int x = bboxmin.x; x <= bboxmax.x; x++) {
            Vec3f bary = barycentric(proj<2>(pts[0]), proj<2>(pts[1]), proj<2>(pts[2]), Vec2f(x, y));
            if (bary.x < 0 || bary.y < 0 || bary.z < 0) continue;
            
            Vec3f zs(pts[0][2], pts[1][2], pts[2][2]);
            float inter_z = zs * bary;

            if (inter_z < zbuffer.get(x, y)) continue;

            TGAColor c;
            bool discard = shader.fragment(bary, c);

            if (discard) c = TGAColor(0, 0, 0, 255);
     
            zbuffer.set(x, y, inter_z);
            image.set(x, y, c);
        }
    }
}
