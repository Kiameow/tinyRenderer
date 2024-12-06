#include "global.h"

Model *model = nullptr;
Vec3f light_dir(0, 0, 1);
Vec3f eye(0, 0, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);
ZBuffer *shadow_buffer = nullptr;
int depth = 255;

