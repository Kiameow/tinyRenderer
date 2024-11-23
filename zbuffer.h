#ifndef __ZBUFFER__
#define __ZBUFFER__

#include <vector>
#include <limits>

class ZBuffer {
    std::vector<float> buffer;
    int width, height;

public:
    ZBuffer(int w, int h) : width(w), height(h), buffer(w * h, -std::numeric_limits<float>::max()) {}
    float get(int x, int y) const { return buffer[y * width + x]; }
    void set(int x, int y, float value) { buffer[y * width + x] = value; }
};

#endif


