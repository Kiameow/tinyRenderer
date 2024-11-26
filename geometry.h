#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <ostream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class t> struct Vec2;
template <class t> struct Vec3;

template <class t> struct Vec2 {
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	Vec2(const Vec3<t>& vec) : u(vec.x), v(vec.y) {}
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
	inline t&      operator [](size_t index) {
		switch (index)
		{
		case 0:
			return u;
		case 1:
			return v;
		default:
            throw std::out_of_range("Index out of range in Vec3");
		}
	}
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	Vec3(const Vec2<t>& vec) : x(vec.u), y(vec.v), z(0) {}
	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	inline t&      operator [](size_t index) {
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
            throw std::out_of_range("Index out of range in Vec3");
		}
	}
	float norm () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

const size_t DEFAULT_ALLOC = 4;

template <class t>
class Matrix {
private: 
	int rows, cols;
	std::vector<std::vector<t>> m; 
public:
	Matrix(size_t r = DEFAULT_ALLOC, size_t c = DEFAULT_ALLOC);

	inline size_t nrows() { return rows; }
	inline size_t ncols() { return cols; }

	Matrix identity(size_t dimensions);
	std::vector<t>& operator[](const int i);
	Matrix operator*(const Matrix& a);
    Matrix transpose();
    Matrix inverse();

    template <typename U>
    friend std::ostream& operator<<(std::ostream& s, const Matrix<U>& m);
};

#endif //__GEOMETRY_H__
