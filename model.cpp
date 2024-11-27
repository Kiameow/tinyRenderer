#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"
#include "tgaimage.h"

Model::Model(const char *obj_filename, const char *texture_filename) : verts_(), texts_(), faces_() {
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
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 3, "vt ")) {
            // vt  0.532 0.923 0.000
            iss >> trash >> trash;
            Vec3f vt;
            for (int i=0;i<3;i++) iss >> vt.raw[i];
            texts_.push_back(vt);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<Vertex> f; 
            int itrash, idx_v, idx_vt;
            iss >> trash;
            while (iss >> idx_v >> trash >> idx_vt >> trash >> itrash) {
                idx_v--; // in wavefront obj all indices start at 1, not zero
                idx_vt--;
                f.push_back(Vertex(idx_v, idx_vt));
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
    load_texture(texture_filename);
}

Model::~Model() {
}

void Model::load_texture(const char *filename) {
    std::string texfile(filename);
    std::cerr << "texture file " << texfile << " loading " << (texture_.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
    texture_.flip_vertically();
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

Vec2i Model::uv(int texture_idx) {
    Vec2f vert_text = texts_[texture_idx];
    return Vec2i(vert_text.x * texture_.get_width(), vert_text.y * texture_.get_height());
}
