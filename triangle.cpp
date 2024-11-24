#include "triangle.h"

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

void triangle(Vec3f *pts, ZBuffer& zbuffer, TGAImage &image, TGAColor* colors) {
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
            Vec3f color = Vec3f(0, 0, 0);
            for (int i=0; i<3; i++) {
                color = color + Vec3f(colors[i].r, colors[i].g, colors[i].b) * bary_coord[i];
            }
            
            TGAColor c = TGAColor(color.x, color.y, color.z, 255);

            if (zbuffer.get(x, y) < z) {
                zbuffer.set(x, y, z);
                image.set(x, y, c);
            }
        }
    }
}

void triangle(Vec3f *pts, ZBuffer& zbuffer, TGAImage &image, Vec3f *texts, TGAImage &texture, float intensity) {
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
            Vec3f text_coord = Vec3f(0, 0, 0);
            for (int i=0; i<3; i++) {
                text_coord = text_coord + texts[i] * bary_coord[i];
            }
            // int u = static_cast<int>(text_coord.x * texture->get_width());
            // int v = texture->get_height() - 1 - static_cast<int>(text_coord.y * texture->get_height());
            TGAColor c = texture.get(text_coord.x, text_coord.y);

            if (zbuffer.get(x, y) < z) {
                zbuffer.set(x, y, z);
                image.set(x, y, c);
            }
        }
    }
}
