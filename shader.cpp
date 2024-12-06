#include "shader.h"
#include "global.h"

Matrix ModelAffine = Matrix::identity();
Matrix ViewAffine = Matrix::identity();  // transform from local coord. to camera coord. 
Matrix ProjectionAffine = Matrix::identity();
Matrix ViewPortAffine = Matrix::identity();   
Matrix Uniform_M = Matrix::identity();
Matrix Uniform_MIT = Matrix::identity();
Matrix ShadowAffineL = Matrix::identity();
Matrix ShadowAffine = Matrix::identity();

void viewport(int x, int y, int width, int height, int depth) {
    ViewPortAffine[0][0] = width  / 2.f;
    ViewPortAffine[1][1] = height / 2.f;
    ViewPortAffine[2][2] = depth  / 2.f;
    
    ViewPortAffine[0][3] = x + width  / 2.f;
    ViewPortAffine[1][3] = y + height / 2.f;
    ViewPortAffine[2][3] = depth      / 2.f;
}

void projection(float coeff=0.f) {
    if (std::abs(coeff) < EPSILON) return;
    ProjectionAffine = Matrix::identity();
    ProjectionAffine[3][2] = -1 / coeff;
}

void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    return ;
    Vec3f z = (eye - center).normalize();
    Vec3f x = cross(up, z).normalize();
    Vec3f y = cross(z, x).normalize();

    Matrix Minv = Matrix::identity();
    Matrix Tr   = Matrix::identity();
    for (int i=0; i<3; i++) {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];
        Tr[i][3] = -eye[i];
    }
    ViewAffine = Minv*Tr;
}

void uniform() {
    Uniform_M = ProjectionAffine * ViewAffine * ModelAffine;
    Uniform_MIT = Uniform_M.invert_transpose();
}

// now it can only rotate by y-axis
void model_affine(Vec3f rel_dest_pos, float rotation_deg, float scale) {
    Matrix R = Matrix::identity();
    Matrix TS = Matrix::identity();
    
    float rotation_radius = rotation_deg / 180 * PI;
    R[0][0] =  std::cos(rotation_radius);
    R[0][2] =  std::sin(rotation_radius); 
    R[2][0] = -std::sin(rotation_radius);
    R[2][2] =  std::cos(rotation_radius);

    TS[0][0] = scale;
    TS[1][1] = scale;
    TS[2][2] = scale;
    TS[0][3] = rel_dest_pos.x;
    TS[1][3] = rel_dest_pos.y;
    TS[2][3] = rel_dest_pos.z;

    ModelAffine = TS * R * ModelAffine;
}

void shadow_left() {
    ShadowAffineL = ViewPortAffine * Uniform_M;
}
void shadow() {
    ShadowAffine = ShadowAffineL * Uniform_MIT * ViewPortAffine.invert();
}

Vec4f GouraudShader::vertex(int face_idx, int vert_idx) {
    if (face_idx >= model->nfaces() || face_idx < 0) throw "face index exceeds the boundary";
    if (vert_idx >= 3 || vert_idx < 0) throw "vertex index must be less than 3 and greater than or equal to 0";

    Vertex vertex = model->face(face_idx)[vert_idx];

    varying_intensity[vert_idx] = std::max(0.f, model->normal(vertex.normal_idx) * light_dir);
    varying_uv.set_col(vert_idx, model->uv(vertex.texture_idx));
    return ViewPortAffine * Uniform_M * embed<4>(model->vert(vertex.vertex_idx));
}

Vec4f PhongShader::vertex(int face_idx, int vert_idx) {
    if (face_idx >= model->nfaces() || face_idx < 0) throw "face index exceeds the boundary";
    if (vert_idx >= 3 || vert_idx < 0) throw "vertex index must be less than 3 and greater than or equal to 0";

    Vertex vertex = model->face(face_idx)[vert_idx];

    varying_normal.set_col(vert_idx, model->normal(vertex.normal_idx));
    varying_uv.set_col(vert_idx, model->uv(vertex.texture_idx));
    Vec4f gl_vert = ViewPortAffine * Uniform_M * embed<4>(model->vert(vertex.vertex_idx));
    varying_vert.set_col(vert_idx, proj<3>(gl_vert / gl_vert[3]));
    return gl_vert;
}

Vec4f NormalBumpShader::vertex(int face_idx, int vert_idx) {
    if (face_idx >= model->nfaces() || face_idx < 0) throw "face index exceeds the boundary";
    if (vert_idx >= 3 || vert_idx < 0) throw "vertex index must be less than 3 and greater than or equal to 0";

    Vertex vertex = model->face(face_idx)[vert_idx];
    varying_uv.set_col(vert_idx, model->uv(vertex.texture_idx));
    return ViewPortAffine * Uniform_M * embed<4>(model->vert(vertex.vertex_idx));
}

Vec4f DepthShader::vertex(int face_idx, int vert_idx) {
    if (face_idx >= model->nfaces() || face_idx < 0) throw "face index exceeds the boundary";
    if (vert_idx >= 3 || vert_idx < 0) throw "vertex index must be less than 3 and greater than or equal to 0";

    Vertex vertex = model->face(face_idx)[vert_idx];
    Vec4f gl_vert = ViewPortAffine * Uniform_M * embed<4>(model->vert(vertex.vertex_idx));
    varying_vert.set_col(vert_idx, proj<3>(gl_vert / gl_vert[3]));
    return gl_vert;
}
