#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

struct Vertex {
	int vertex_idx;
	int texture_idx;
	int normal_idx;

	Vertex(int vidx, int tidx = 0, int nidx = 0) : vertex_idx(vidx), texture_idx(tidx), normal_idx(nidx) {}
};

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> normals_;
	std::vector<Vec2f> texts_;
	std::vector<std::vector<Vertex>> faces_;
	TGAImage texture_;
	TGAImage normal_texture_;
	TGAImage spec_texture_;
	void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
	Model(const char *obj_filename);
	~Model();
	int nverts();
	int ntexts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<Vertex> face(int idx);
	Vec3f normal(int i);
	Vec2i uv(int texture_idx);
	inline TGAColor diffuse(int u, int v) {	return texture_.get(u, v); }
	inline TGAColor getNormal(int u, int v) { return normal_texture_.get(u , v); }
};

#endif //__MODEL_H__
