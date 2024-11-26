#include "triangle.h"
#include "global.h"
#include <iostream>

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
    Vec3f c1 = Vec3f(B.x - A.x, C.x - A.x, A.x - P.x);
    Vec3f c2 = Vec3f(B.y - A.y, C.y - A.y, A.y - P.y);

    Vec3f u = c1 ^ c2;
    if (std::abs(u.z) < EPSILON) {
        // invalid output
        return Vec3f(-1, 0, 0);
    }
    Vec3f un = Vec3f(u.x / u.z, u.y / u.z, 1);
    if (un.x < 0 || un.y < 0 || un.x + un.y > 1)  return Vec3f(-1, 0, 0);
    return Vec3f(1 - (un.x + un.y), un.x, un.y);
}

void triangle(Vec3f *pts, Vec2i *texts, float intensity, ZBuffer &zbuffer, TGAImage &image) {
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
            float z = - std::numeric_limits<float>::max();
        
            Vec3f bary_coord = barycentric(pts[0], pts[1], pts[2], Vec3f(x, y, 0));
            if (bary_coord.x < 0 || bary_coord.y < 0 || bary_coord.z < 0) continue;
            
            z = bary_coord * Vec3f(pts[0].z, pts[1].z, pts[2].z);

            float u = 0, v = 0;
            for (int i = 0; i < 3; i++) {
                u += texts[i].u * bary_coord[i];
                v += texts[i].v * bary_coord[i];
            }

            std::cout << u << " " << v << std::endl;

            TGAColor c = model->diffuse(u, v);
            // std::cout << ":" << std::endl;
            // std::cout << c.r << c.g << std::endl;
            for (int i = 0; i < 3; i++) {
                c.r *= intensity;
                c.g *= intensity;
                c.b *= intensity;
            }

            if (zbuffer.get(x, y) < z) {
                zbuffer.set(x, y, z);
                image.set(x, y, c);
            }
        }
    }
}
