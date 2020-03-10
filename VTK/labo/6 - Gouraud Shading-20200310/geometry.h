#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <ostream>
#include <array>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct Vec2 {
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
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
	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
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

template<size_t rows, size_t cols>
class Matrix {
    std::array<std::array<float,cols>,rows> m;
public:
    Matrix(float val = 0.f) {
        for(auto& row : m)
            row.fill(val);
    };

    std::array<float,cols>& operator[](size_t i) {
        assert(i < rows);
        return m[i];
    };

    const std::array<float,cols>& operator[](size_t i) const {
        assert(i < rows);
        return m[i];
    };

    template<size_t N>
    Matrix<rows,N> operator*(const Matrix<cols,N>& a) {
        Matrix<rows,N> result;
        for (int i=0; i<rows; i++) {
            for (int j=0; j<N; j++) {
                for (int k=0; k<cols; k++) {
                    result[i][j] += m[i][k] * a[k][j];
                }
            }
        }
        return result;
    }

    Matrix<cols,rows> transpose() {
        Matrix<cols,rows> result;
        for(int i=0; i<rows; i++)
            for(int j=0; j<cols; j++)
                result[j][i] = m[i][j];
        return result;
    }

    Matrix inverse() {
        static_assert(rows == cols, "Only square matrices can be inverted");

        // augmenting the square matrix with the identity matrix of the same dimensions a => [ai]
        Matrix<rows,cols*2> result;
        for(int i=0; i<rows; i++)
            for(int j=0; j<cols; j++)
                result[i][j] = m[i][j];
        for(int i=0; i<rows; i++)
            result[i][i+cols] = 1;
        // first pass
        for (int i=0; i<rows-1; i++) {
            // normalize the first row
            for(int j=cols*2-1; j>=0; j--)
                result[i][j] /= result[i][i];
            for (int k=i+1; k<rows; k++) {
                float coeff = result[k][i];
                for (int j=0; j<cols*2; j++) {
                    result[k][j] -= result[i][j]*coeff;
                }
            }
        }
        // normalize the last row
        for(int j=cols*2-1; j>=rows-1; j--)
            result[rows-1][j] /= result[rows-1][rows-1];
        // second pass
        for (int i=rows-1; i>0; i--) {
            for (int k=i-1; k>=0; k--) {
                float coeff = result[k][i];
                for (int j=0; j<cols*2; j++) {
                    result[k][j] -= result[i][j]*coeff;
                }
            }
        }
        // cut the identity matrix back
        Matrix<rows,cols> truncate;
        for(int i=0; i<rows; i++)
            for(int j=0; j<cols; j++)
                truncate[i][j] = result[i][j+cols];
        return truncate;
    }

    static Matrix identity() {
        static_assert(rows == cols, "Only square matrices can be identity");
        Matrix m;
        for(size_t i = 0; i < rows; ++i)
            m[i][i] = 1.f;
        return m;
    }
};

using Matrix4x4 = Matrix<4,4>;
using VecH = Matrix<4,1>;

template<size_t rows, size_t cols>
std::ostream& operator<<(std::ostream& s, const Matrix<rows,cols>& m) {
    for (int i=0; i< rows; i++)  {
        for (int j=0; j< cols; j++) {
            s << m[i][j];
            if (j<cols-1) s << "\t";
        }
        s << "\n";
    }
    return s;
}

#endif //__GEOMETRY_H__
