#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "model.h"
#include "geometry.h"
#include "zbuffer.h"

#define EPSILON 0.001f
#define PI 3.1415926

extern Model *model;
extern Vec3f light_dir;
extern Vec3f eye;
extern Vec3f center;
extern Vec3f up;
extern ZBuffer *shadow_buffer;
extern int depth;

#endif
