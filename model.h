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
	void load_texture(const char *filename);
public:
	Model(const char *obj_filename, const char *texture_filename);
	~Model();
	int nverts();
	int ntexts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<Vertex> face(int idx);
	Vec2i uv(int texture_idx);
	inline TGAColor diffuse(int u, int v) {	return texture_.get(u, v); }
};

#endif //__MODEL_H__
