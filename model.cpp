#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"
#include "tgaimage.h"

Model::Model(std::string obj_filename) : verts_(), texts_(), faces_() {
    std::ifstream in;
    in.open (obj_filename, std::ifstream::in);
    if (in.fail()) {
        std::cerr << "Open " << obj_filename << " failed!" << std::endl;
        return;
    }
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 3, "vt ")) {
            // vt  0.532 0.923 0.000
            iss >> trash >> trash;
            Vec2f vt;
            for (int i=0;i<2;i++) iss >> vt[i];
            texts_.push_back(vt);
        } else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f vn;
            for (int i=0;i<3;i++) iss >> vn[i];
            normals_.push_back(vn);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<Vertex> f; 
            int idx_v, idx_vt, idx_vn;
            iss >> trash;
            while(iss >> idx_v >> trash >> idx_vt >> trash >> idx_vn) {
                idx_v--; // in wavefront obj all indices start at 1, not zero
                idx_vt--;
                idx_vn--;

                f.push_back(Vertex(idx_v, idx_vt, idx_vn));
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
    load_texture(obj_filename, "_diffuse.tga", texture_);
    load_texture(obj_filename, "_nm.tga", normal_texture_);
    load_texture(obj_filename, "_spec.tga", spec_texture_);
}

Model::~Model() {
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot!=std::string::npos) {
        texfile = texfile.substr(0,dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::ntexts() {
    return (int)texts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<Vertex> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::normal(int i) {
    return normals_[i];
}

Vec2f Model::uv(int texture_idx) {
    return texts_[texture_idx];
}

TGAColor Model::diffuse(Vec2f uvf) {
    Vec2i uv(uvf[0]*texture_.get_width(), uvf[1]*texture_.get_height());
    return texture_.get(uv[0], uv[1]);
}

Vec3f Model::nm(Vec2f uvf) {
    Vec2i uv(uvf[0]*normal_texture_.get_width(), uvf[1]*normal_texture_.get_height());
    TGAColor c = normal_texture_.get(uv[0], uv[1]);
    Vec3f res;
    for (int i=0; i<3; i++)
        res[i] = (float)c[i]/255.f*2.f - 1.f;
    return res;
}

float Model::spec(Vec2f uvf) {
    Vec2i uv(uvf[0]*spec_texture_.get_width(), uvf[1]*spec_texture_.get_height());
    TGAColor c = spec_texture_.get(uv[0], uv[1]);
    return c[0] / 1.f;
}
