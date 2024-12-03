#include "shader.h"
#include "global.h"

Matrix ModelView = Matrix::identity();
Matrix ViewPort = Matrix::identity();
Matrix Projection = Matrix::identity();
Matrix Uniform_M = Matrix::identity();
Matrix Uniform_MIT = Matrix::identity();

void viewport(int x, int y, int width, int height, int depth) {
    ViewPort[0][0] = width  / 2.f;
    ViewPort[1][1] = height / 2.f;
    ViewPort[2][2] = depth  / 2.f;
    
    ViewPort[0][3] = x + width  / 2.f;
    ViewPort[1][3] = y + height / 2.f;
    ViewPort[2][3] = depth      / 2.f;
    //std::cout << ViewPort << std::endl;
}

void projection(float coeff=0.f) {
    if (std::abs(coeff) < EPSILON) return;
    Projection[3][2] = -1 / coeff;
}

void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    return ;
    Vec3f z = (eye - center).normalize();
    Vec3f x = cross(up, z).normalize();
    Vec3f y = cross(z, x).normalize();

    // not understand
    Matrix Minv = Matrix::identity();
    Matrix Tr   = Matrix::identity();
    for (int i=0; i<3; i++) {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];
        Tr[i][3] = -eye[i];
    }
    ModelView = Minv*Tr;
}

void uniform() {
    Uniform_M = Projection * ModelView;
    Uniform_MIT = Uniform_M.invert_transpose();
}

void rotate(float rotation_degree) {
    Matrix M = Matrix::identity();
    
    float rotation_radius = rotation_degree / 180 * PI;
    M[0][0] =  std::cos(rotation_radius);
    M[0][2] =  std::sin(rotation_radius);
    M[2][0] = -std::sin(rotation_radius);
    M[2][2] =  std::cos(rotation_radius);
    ModelView = M * ModelView;
}

Vec4f GouraudShader::vertex(int face_idx, int vert_idx) {
    if (face_idx >= model->nfaces() || face_idx < 0) throw "face index exceeds the boundary";
    if (vert_idx >= 3 || vert_idx < 0) throw "vertex index must be less than 3 and greater than or equal to 0";

    Vertex vertex = model->face(face_idx)[vert_idx];

    varying_intensity[vert_idx] = std::max(0.f, model->normal(vertex.normal_idx) * light_dir);
    varying_uv.set_col(vert_idx, model->uv(vertex.texture_idx));
    return ViewPort * Projection * ModelView * embed<4>(model->vert(vertex.vertex_idx));
}

Vec4f PhongShader::vertex(int face_idx, int vert_idx) {
    if (face_idx >= model->nfaces() || face_idx < 0) throw "face index exceeds the boundary";
    if (vert_idx >= 3 || vert_idx < 0) throw "vertex index must be less than 3 and greater than or equal to 0";

    Vertex vertex = model->face(face_idx)[vert_idx];

    Vec4f normal = Uniform_MIT * embed<4>(model->normal(vertex.normal_idx));
    normal = (normal / normal[3]);
    varying_normal.set_col(vert_idx, proj<3>(normal).normalize());
    varying_uv.set_col(vert_idx, model->uv(vertex.texture_idx));
    return ViewPort * Projection * ModelView * embed<4>(model->vert(vertex.vertex_idx));
}

Vec4f NormalBumpShader::vertex(int face_idx, int vert_idx) {
    if (face_idx >= model->nfaces() || face_idx < 0) throw "face index exceeds the boundary";
    if (vert_idx >= 3 || vert_idx < 0) throw "vertex index must be less than 3 and greater than or equal to 0";

    Vertex vertex = model->face(face_idx)[vert_idx];
    varying_uv.set_col(vert_idx, model->uv(vertex.texture_idx));
    return ViewPort * Projection * ModelView * embed<4>(model->vert(vertex.vertex_idx));
}
