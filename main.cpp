#include "tgaimage.h"
#include "model.h"
#include <iostream>

#define IMAGE_WIDTH 100
#define IMAGE_HEIGHT 100

#define LARGE_IMAGE_WIDTH 800
#define LARGE_IMAGE_HEIGHT 800

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

void triangle(Vec2f v0, Vec2f v1, Vec2f v2, TGAImage &image, TGAColor color) {
    line(v0.x, v0.y, v1.x, v1.y, image, color);
    line(v0.x, v0.y, v2.x, v2.y, image, color);
    line(v2.x, v2.y, v1.x, v1.y, image, color);

    // sort vertecies by the y coordinates
    if (v0.y > v1.y) std::swap(v0, v1);
    if (v1.y > v2.y) std::swap(v1, v2);
    if (v0.y > v1.y) std::swap(v0, v1);

    float k1 = getLineSteep(v0, v1);
    float k2 = getLineSteep(v0, v2);

    for (int y = v0.y; y <= v1.y; y++) {
        int x1 = v0.x + (y - v0.y) / k1;
        int x2 = v0.x + (y - v0.y) / k2;
        line(x1, y, x2, y, image, color);
    }

    float k3 = getLineSteep(v1, v2);

    for (int y = v2.y; y >= v1.y; y--) {
        int x2 = v2.x + (y - v2.y) / k2;
        int x3 = v2.x + (y - v2.y) / k3;
        line(x2, y, x3, y, image, color);
    }
}

Vec3f barycentric(Vec2i *pts, Vec2i P) {
    Vec3i bary = Vec3i(pts[1].x - pts[0].x, pts[2].x - pts[0].x, pts[0].x - P.x) ^ Vec3i(pts[1].y - pts[0].y, pts[2].y - pts[0].y, pts[0].y - P.y);

    // z == 0 means the triangle is degenerated
    if (std::abs(bary.z) == 0) return Vec3f(-1.0, 0.0, 0.0);
    
    float fz = float(bary.z);
    return Vec3f(1.f - (bary.x + bary.y) / fz, bary.x / fz, bary.y / fz);
}

void triangle(Vec2i *pts, TGAImage &image, TGAColor color) {
    Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(image.get_width() - 1, image.get_height() - 1);

    for (int i = 0; i < 3; i++)
    {
        bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

        bboxmax.x = std::min(std::max(bboxmax.x, pts[i].x), clamp.x);
        bboxmax.y = std::min(std::max(bboxmax.y, pts[i].y), clamp.y);
    }
    
    for (int y = bboxmin.y; y <= bboxmax.y; y++) {
        for (int x = bboxmin.x; x <= bboxmax.x; x++) {
            Vec3f bary = barycentric(pts, Vec2i(x, y));
            if (bary.x < 0 || bary.y < 0 || bary.z < 0) continue;
            image.set(x, y, color);
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
    image.clear();
    Vec2i pts[3] = {Vec2i(150, 20), Vec2i(10, 50), Vec2i(80, 80)};
    triangle(pts, image, red);
    triangle(Vec2f(30, 10), Vec2f(90, 30), Vec2f(50, 25), image, blue);
    image.write_tga_file("./images/triangle_v2.tga");
    #endif


    #ifdef MODEL_TEST
    image.clear();
    int width = LARGE_IMAGE_WIDTH;   
    int height = LARGE_IMAGE_WIDTH;   
    image.scale(width, height);
    Model *model = new Model("./objs/afraican_head.obj");
    for (int i=0; i<model->nfaces(); i++) { 
        std::vector<int> face = model->face(i); 
        Vec2i screen_coords[3]; 
        for (int j=0; j<3; j++) { 
            Vec3f world_coords = model->vert(face[j]); 
            screen_coords[j] = Vec2i((world_coords.x+1.)*width/2., (world_coords.y+1.)*height/2.); 
        } 
        triangle(screen_coords, image, TGAColor(rand()%255, rand()%255, rand()%255, 255)); 
    }
    image.flip_vertically();
    image.write_tga_file("./images/colorful_head_model.tga");
    #endif    
    
    return 0;
}
