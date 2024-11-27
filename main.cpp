#include "tgaimage.h"
#include "global.h"
#include "line.h"
#include "model.h"
#include "triangle.h"
#include "zbuffer.h"
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
    
    Vec3f light_dir(0, 0, -1);
    

    light_dir.normalize();
    int width = LARGE_IMAGE_WIDTH;   
    int height = LARGE_IMAGE_WIDTH;   
    image.scale(width, height);
    model = new Model("../objs/afraican_head.obj", "../texture/african_head_diffuse.tga");
    ZBuffer zbuffer(width, height);

    for (int i=0; i<model->nfaces(); i++) { 
        std::vector<Vertex> face = model->face(i); 
        Vec3f world_coords[3];
        Vec2i text_coords[3];
        TGAColor colors[3];
        for (int j=0; j<3; j++) { 
            Vec3f v  = model->vert(face[j].vertex_idx); 
            Vec2i vt = model->uv(face[j].texture_idx);
            world_coords[j] = Vec3f((v.x + 1) * width / 2, (v.y + 1) * height / 2, v.z);
            text_coords[j] = vt;
        } 
        // question: normal_face direction can be various, let's just ignore that for now
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = light_dir * n;

        if (intensity <= 0) continue;
        //triangle(world_coords, zbuffer, image, colors); 
        triangle(world_coords, text_coords, intensity, zbuffer, image);
    }
    image.flip_vertically();
    image.write_tga_file("../images/text_head_model.tga");
    #endif    
    
    return 0;
}
