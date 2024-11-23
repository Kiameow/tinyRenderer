#include "tgaimage.h"
#include "model.h"
#include "zbuffer.h"
#include <iostream>

#define IMAGE_WIDTH 100
#define IMAGE_HEIGHT 100

#define LARGE_IMAGE_WIDTH 800
#define LARGE_IMAGE_HEIGHT 800

#define EPSILON 0.001f

#define LINE_TES
#define TRIANGLE_TES
#define MODEL_TEST

TGAColor red(255, 0, 0, 255);
TGAColor green(0, 255, 0, 255);
TGAColor blue(0, 0, 255, 255);
TGAColor white(255, 255, 255, 255);
TGAImage image(IMAGE_WIDTH, IMAGE_HEIGHT, TGAImage::RGB);

namespace lesson0 {
    void draw_dot(TGAImage &image) {
        image.set(10, 50, red);
        image.flip_horizontally();
    }
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    // when y distance is larger than the x distance, then it's steep, swap the x,y
    if (std::abs(y1 - y0) > std::abs(x1 - x0)) {
        steep = true;
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    // now dy must be smaller than dx
    int dx = x1 - x0;
    int dy = y1 - y0;

    int derror2 = std::abs(dy) * 2;
    int error2 = 0;

    for (int x = x0, y = y0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

float getLineSteep(Vec2f v0, Vec2f v1) {
    return (v1.y - v0.y) / float(v1.x - v0.x);
}

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

void triangle(Vec3f *pts, ZBuffer& zbuffer, TGAImage &image, TGAColor color) {
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
            if (x == 79 && y == 20) {
                std::cout << bary_coord << std::endl;
            }
            if (bary_coord.x < 0 || bary_coord.y < 0 || bary_coord.z < 0) continue;
            
            z = bary_coord * Vec3f(pts[0].z, pts[1].z, pts[2].z);

            if (zbuffer.get(x, y) < z) {
                zbuffer.set(x, y, z);
                image.set(x, y, color);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    #ifdef LINE_TEST
    image.clear();
    line(10, 10, 20, 90, image, green);
    line(90, 90, 10, 10, image, red);
    line(20, 90, 90, 50, image, blue);
    image.write_tga_file("./images/line.tga");
    #endif
    
    #ifdef TRIANGLE_TEST
    ZBuffer zbuffer(IMAGE_WIDTH, IMAGE_HEIGHT);
    image.clear();
    Vec3f pts[3] = {Vec3f(50, 20, 0), Vec3f(10, 50, 0), Vec3f(80, 80, 0)};
    triangle(pts, zbuffer, image, red);
    // triangle(Vec2f(30, 10), Vec2f(90, 30), Vec2f(50, 25), image, blue);
    image.write_tga_file("./images/triangle_v2.tga");
    #endif


    #ifdef MODEL_TEST
    image.clear();
    // Vec3f light = Vec3f(100, 100, 0);
    Vec3f light_dir(0, 0, -1);
    light_dir.normalize();
    int width = LARGE_IMAGE_WIDTH;   
    int height = LARGE_IMAGE_WIDTH;   
    image.scale(width, height);
    Model *model = new Model("./objs/afraican_head.obj");
    ZBuffer zbuffer(width, height);

    for (int i=0; i<model->nfaces(); i++) { 
        std::vector<int> face = model->face(i); 
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) { 
            Vec3f v = model->vert(face[j]); 
            world_coords[j] = Vec3f((v.x + 1) * width / 2, (v.y + 1) * height / 2, v.z * 500);
        } 
        // question: normal_face direction can be various, let's just ignore that for now
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = light_dir * n;

        if (intensity > 0) {
            triangle(world_coords, zbuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255)); 
        }
    }
    image.flip_vertically();
    image.write_tga_file("./images/shadow_head_model_fixed_v2.tga");
    #endif    
    
    return 0;
}
