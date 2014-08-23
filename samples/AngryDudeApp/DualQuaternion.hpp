#ifndef __DualQuaternion_hpp__
#define __DualQuaternion_hpp__

#include <cmath>
#include <type_traits>

class Quaternion
{
public:
    Quaternion() = default;
    Quaternion(float x, float y, float z, float w): x(x), y(y), z(z), w(w) {}

    template <typename Q,
              typename std::enable_if<not std::is_floating_point<Q>::value>::type* = nullptr>
    Quaternion(const Q& q): x(q.x), y(q.y), z(q.z), w(q.w) {}
    /// This constructor is meant to simplify working with different implementations of quaternions.

    template <typename F,
              typename std::enable_if<std::is_floating_point<F>::value>::type* = nullptr>
    Quaternion(F f): x(0.f), y(0.f), z(0.f), w(f) {}
    /// Real numbers are a subset of quaternions (when x, y and z are zero).
    /// Constructor above is enabled when F is a floating point.
    /// For example
    /// 2.f * Quaternion()
    /// is actually implemented as
    /// Quaternion(2.f) * Quaternion()

    template <typename V3>
    Quaternion(const V3& unitAxis, float thetaRadians)
    {
        const float halfAngle = thetaRadians / 2.f;
        const float sa = std::sin(halfAngle);
        const float ca = std::cos(halfAngle);
        x = sa*unitAxis.x;
        y = sa*unitAxis.y;
        z = sa*unitAxis.z;
        w = ca;
    }

    static const Quaternion identity()
    {
        return Quaternion(0.f, 0.f, 0.f, 1.f);
    }

    template <typename V3>
    static const V3 toVector(const Quaternion& q)
    {
        return V3(q.x, q.y, q.z);
    }

    template <typename V3>
    static const Quaternion fromVector(const V3& v)
    {
        /// A pure quaternion can represent a 3D vector (_not_ translation).
        /// It turns out that the product q * Quaternion::fromVector(v) * conjugate(q),
        /// where q is unit quaternion, rotates
        /// the vector v (in quaternion representation).
        return Quaternion(v.x, v.y, v.z, 0.f);
    }

    template <typename M3>
    static const M3 toMatrix(const Quaternion& q)
    {
        const float x = q.x;
        const float y = q.y;
        const float z = q.z;
        const float w = q.w;
        M3 m;
        // Assuming M3 has an operator() (row, column).
        m(0,0) = 1.f - 2.f*(y*y + z*z);
        m(0,1) = 2.f*(x*y - w*z);
        m(0,2) = 2.f*(x*z + w*y);
             
        m(1,0) = 2.f*(x*y + w*z);
        m(1,1) = 1.f - 2.f*(x*x + z*z);
        m(1,2) = 2.f*(y*z - w*x);
             
        m(2,0) = 2.f*(x*z - w*y);
        m(2,1) = 2.f*(y*z + w*x);
        m(2,2) = 1.f - 2.f*(x*x + y*y);
        return m;
    }

    template <typename M3>
    static const Quaternion fromMatrix(const M3& m)
    {
        /// Assumes that matrix m represents pure rotation.
        /// Converts to unit quaternion.
        Quaternion q;
        const float trace = m(0,0) + m(1,1) + m(2,2);
        if (trace > 0.f) {
            const float s = std::sqrt(trace + 1.f) * 2.f;
            q.w = 0.25f * s;
            q.x = (m(2,1) - m(1,2)) / s;
            q.y = (m(0,2) - m(2,0)) / s;
            q.z = (m(1,0) - m(0,1)) / s;
        } else if ((m(0,0) > m(1,1)) && (m(0,0) > m(2,2))) {
            const float s = std::sqrt(m(0,0) - m(1,1) - m(2,2) + 1.f) * 2.f;
            q.w = (m(2,1) - m(1,2)) / s;
            q.x = 0.25f * s;
            q.y = (m(0,1) + m(1,0)) / s;
            q.z = (m(0,2) + m(2,0)) / s;
        } else if (m(1,1) > m(2,2)) {
            const float s = std::sqrt(m(1,1) - m(0,0) - m(2,2) + 1.f) * 2.f;
            q.w = (m(0,2) - m(2,0)) / s;
            q.x = (m(0,1) + m(1,0)) / s;
            q.y = 0.25f * s;
            q.z = (m(1,2) + m(2,1)) / s;
        } else {
            const float s = std::sqrt(m(2,2) - m(0,0) - m(1,1) + 1.f) * 2.f;
            q.w = (m(1,0) - m(0,1)) / s;
            q.x = (m(0,2) + m(2,0)) / s;
            q.y = (m(1,2) + m(2,1)) / s;
            q.z = 0.25f * s;
        }
        return q;
    }

    float x,y,z,w;
};

inline const Quaternion operator + (const Quaternion& a, const Quaternion& b)
{
    return Quaternion(a.x + b.x,
                      a.y + b.y,
                      a.z + b.z,
                      a.w + b.w);
}

inline const Quaternion operator * (const Quaternion& a, const Quaternion& b)
{
    return Quaternion(a.y*b.z - a.z*b.y + a.w*b.x + a.x*b.w,
                      a.z*b.x - a.x*b.z + a.w*b.y + a.y*b.w,
                      a.x*b.y - a.y*b.x + a.w*b.z + a.z*b.w,
                      a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z);
}

inline const Quaternion conjugate(const Quaternion& q)
{
    return Quaternion(-q.x, -q.y, -q.z, q.w);
}

class DualQuaternion
{
public:
    DualQuaternion() = default;
    DualQuaternion(const Quaternion& real, const Quaternion& dual):
        real(real), dual(dual) {}

    template <typename V3>
    DualQuaternion(const V3& translation, const Quaternion& rotation)
    {
        /// Constructs unit dual quaternion encoding rotation and translation (after rotation).
        real = rotation;
        dual = Quaternion(translation.x / 2.f, translation.y / 2.f, translation.z / 2.f, 0.f) * real;
    }

    static const DualQuaternion identity()
    {
        return DualQuaternion(Quaternion(0.f, 0.f, 0.f, 1.f),
                              Quaternion(0.f, 0.f, 0.f, 0.f));
    }

    template <typename V3>
    static const DualQuaternion fromVector(const V3& vector)
    {
        return DualQuaternion(Quaternion(0.f, 0.f, 0.f, 1.f),
                              Quaternion(vector.x, vector.y, vector.z, 0.f));
    }

    template <typename V3>
    static const V3 toVector(const DualQuaternion& dq)
    {
        return V3(dq.dual.x, dq.dual.y, dq.dual.z);
    }

    template <typename M4>
    static const DualQuaternion fromMatrix(const M4& m)
    {
        const Quaternion rotation = Quaternion::fromMatrix(m);
        const Quaternion translation = Quaternion(m(0,3) / 2.f,
                                                  m(1,3) / 2.f,
                                                  m(2,3) / 2.f,
                                                  0.f);
        return DualQuaternion(rotation, translation*rotation);
    }

    template <typename M4>
    static const M4 toMatrix(const DualQuaternion& dq)
    {
        M4 m = Quaternion::toMatrix<M4>(dq.real);
        const Quaternion translation = 2.f * dq.dual * conjugate(dq.real);
        m(0,3) = translation.x;
        m(1,3) = translation.y;
        m(2,3) = translation.z;

        m(3,0) = 0.f;
        m(3,1) = 0.f;
        m(3,2) = 0.f;
        m(3,3) = 1.f;
        return m;
    }

    Quaternion real, dual;
};
static_assert(sizeof(DualQuaternion) == 8*4, "DualQuaternion is of unexpected size!");
static_assert(std::is_pod<DualQuaternion>::value == true, "DualQuaternion not POD!");
static_assert(std::is_trivial<DualQuaternion>::value == true, "DualQuaternion not trivial!");
static_assert(std::is_standard_layout<DualQuaternion>::value == true, "DualQuaternion not standard layout!");
/// In short:
/// the idea of POD (plain old datatypes) classes is to support static initialization and
/// have the same memory layout as in C.
/// C++11 says a class is POD if it is both trivial and standard layout.
/// It is ok to use memcpy over trivial classes.
/// Standard layout class has the same memory layout as the equivalent C struct.

inline const DualQuaternion operator*(const DualQuaternion& a, const DualQuaternion& b)
{
    /// a*b = (a0 + eps*ae)*(b0 + eps*be)
    ///     = a0*b0 + eps*(a0*be + ae*b0)
    ///
    return DualQuaternion(a.real*b.real,
                          a.real*b.dual + a.dual*b.real);
}

inline const DualQuaternion conjugateDual(const DualQuaternion& dq)
{
    /// Both quaternion and dual number conjugation.
    return DualQuaternion(conjugate(dq.real),
                          Quaternion(dq.dual.x, dq.dual.y, dq.dual.z, -dq.dual.w));
}

#endif
