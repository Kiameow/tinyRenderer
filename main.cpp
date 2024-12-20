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

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 800

#define LARGE_IMAGE_WIDTH 1600
#define LARGE_IMAGE_HEIGHT 1600

#define LINE_TES
#define TRIANGLE_TES
#define MODEL_TEST

TGAColor red(255, 0, 0, 255);
TGAColor green(0, 255, 0, 255);
TGAColor blue(0, 0, 255, 255);
TGAColor white(255, 255, 255, 255);

int main(int argc, char* argv[]) {
    std::string model_name = "afraican_head";
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
        {"-m", [&](int& i) {
            if (i + 1 < argc) {
                model_name = argv[++i];
                images_folder = images_folder + model_name + "/";
            } else {
                std::cerr << "Error: Missing value for -m option." << std::endl;
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
    };

    if (argc > 1) {
        for (int i=1; i<argc; i++) {
            std::string arg = argv[i];
            if (options.find(arg) != options.end()) {
                options[arg](i);
            }
        }
    }
    std::string model_filename = "../objs/" + model_name + "/" + model_name + ".obj";

    #ifdef MODEL_TEST
    
    int width = LARGE_IMAGE_WIDTH;   
    int height = LARGE_IMAGE_WIDTH;  

    model = new Model(model_filename);

    //-------------Shadow Mapping--------------//
    TGAImage depth_image(width, height, TGAImage::RGB);

    light_dir = Vec3f(1, 1, 1).normalize();
    eye = light_dir * 3;
    up = Vec3f(0, 1, 0);
    center = Vec3f(0, 0, 0);
    
    viewport(width/8, height/8, width * 3/4, height * 3/4, depth);
    projection(-1 / (eye - center).norm());
    model_affine(Vec3f(0,0,0), 0, 1);
    lookat(eye, center, up);
    uniform();
    shadow();

    shadow_buffer = new ZBuffer(width, height);

    DepthShader depthShader;
    
    for (int i=0; i<model->nfaces(); i++) { 
        Vec4f screen_coords[3];
    
        for (int j=0; j<3; j++) { 
            screen_coords[j] = depthShader.vertex(i, j);
            screen_coords[j] = screen_coords[j] / screen_coords[j][3];
        } 
        
        triangle(screen_coords, depthShader, *shadow_buffer, depth_image);
    }
    depth_image.flip_vertically();
    depth_image.write_tga_file((images_folder + "depth.tga").c_str());
    //-------------Shadow Mapping End--------------//

    //-------------Shading---------------//
    TGAImage image(width, height, TGAImage::RGB);
    ZBuffer zbuffer(width, height);

    eye = Vec3f(1, 1, 2);
    std::cout << Uniform_M << std::endl;

    viewport(width/8, height/8, width * 3/4, height * 3/4, depth);
    projection(-1 / (eye - center).norm());
    model_affine(Vec3f(0,0,0), rotation_degree, 1);
    lookat(eye, center, up);
    uniform();

    // std::cout << ProjectionAffine << std::endl;
    // std::cout << "ViewAffine" << std::endl;
    // std::cout << ViewAffine << std::endl;
    // std::cout << Uniform_M << std::endl;


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
