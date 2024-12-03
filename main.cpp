#include "tgaimage.h"
#include "global.h"
#include "line.h"
#include "model.h"
#include "shader.h"
#include "triangle.h"
#include "zbuffer.h"
#include <cmath>
#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <functional>
#include <memory>

#define IMAGE_WIDTH 100
#define IMAGE_HEIGHT 100
#define DEPTH 127

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
    std::string output_filename = "output.tga";
    std::string images_folder = "../images/";
    float rotation_degree = 0.f;
    std::unique_ptr<IShader> shader = std::make_unique<GouraudShader>();
    
    std::unordered_map<std::string, std::function<void(int&)>> options{
        {"-o", [&](int& i) {
            if (i + 1 < argc) {
                output_filename = argv[++i];
                std::cout << "Output filename: " << output_filename << std::endl;
            } else {
                std::cerr << "Error: Missing value for -o option." << std::endl;
            }
        }},
        {"--rotate", [&](int& i) { 
            if (i + 1 < argc) {
                rotation_degree = atof(argv[++i]);
            } else {
                std::cerr << "Error: Missing value for --rotate option." << std::endl;
            }
        }},
        {"--goraud", [&](int&) { shader = std::make_unique<GouraudShader>(); }},
        {"--phong", [&](int&) { shader = std::make_unique<PhongShader>(); }},
        {"--bump", [&](int&) { shader = std::make_unique<NormalBumpShader>(); }},
    };

    if (argc > 1) {
        for (int i=1; i<argc; i++) {
            std::string arg = argv[i];
            if (options.find(arg) != options.end()) {
                options[arg](i);
            }
        }
    }

    #ifdef MODEL_TEST
    image.clear();
    
    light_dir.normalize();
    
    int width = LARGE_IMAGE_WIDTH;   
    int height = LARGE_IMAGE_WIDTH;  
    int depth = DEPTH; 

    image.scale(width, height);

    viewport(width/8, height/8, width * 3/4, height * 3/4, depth);
    projection(eye.z);
    rotate(rotation_degree);
    lookat(eye, center, up);
    uniform();

    model = new Model("../objs/afraican_head.obj", "../texture/african_head_diffuse.tga", "../texture/african_head_nm.tga");
    ZBuffer zbuffer(width, height);

    for (int i=0; i<model->nfaces(); i++) { 
        Vec4f screen_coords[3];
    
        for (int j=0; j<3; j++) { 
            screen_coords[j] = shader->vertex(i, j);
            screen_coords[j] = screen_coords[j] / screen_coords[j][3];
        } 
        
        triangle(screen_coords, *shader, zbuffer, image);
    }
    image.flip_vertically();
    image.write_tga_file((images_folder + output_filename).c_str());
    #endif    
    
    return 0;
}

// float rotation_radius = rotation_degree / 180 * PI;
    // ModelView[0][0] =  std::cos(rotation_radius);
    // ModelView[0][2] =  std::sin(rotation_radius);
    // ModelView[2][0] = -std::sin(rotation_radius);
    // ModelView[2][2] =  std::cos(rotation_radius);

    // 0  0  1  0    x      z
    // 0  1  0  0    y      y
    //-1  0  0  0  * z  =  -x
    // 0  0  0  1    1      1

    // although I changed the vertex position via rotation, but the normal of each vertex
    // still not changed(which is defined in obj file), in triangle rasterizing, I 
    // firstly dot product the normal and
    // light direction, if it's less than zero, then it will be skipped, so there might 
    // some issue, that some vertex shouldb't be skipped, but skipped

    // so the solution will be rotation also be applied to normal, which is the transpose
    // of the ModelView Matrix

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
