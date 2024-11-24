#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

struct Vertex {
	int vertex_idx;
	int texture_idx;
	int normal_idx;

	Vertex(int vidx, int tidx = 0, int nidx = 0) : vertex_idx(vidx), texture_idx(tidx), normal_idx(nidx) {}
};

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> texts_;
	std::vector<std::vector<Vertex>> faces_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int ntexts();
	int nfaces();
	Vec3f vert(int i);
	Vec3f text(int i);
	std::vector<Vertex> face(int idx);
};

#endif //__MODEL_H__
