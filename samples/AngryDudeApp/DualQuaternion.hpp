#ifndef __DualQuaternion_hpp__
#define __DualQuaternion_hpp__

#include <cmath>
#include <type_traits>

/// \brief Quaternions are an extension of real numbers
/// used to represent rotations in 3D.
///
/// Quaternions are an alternative to Euler angles or rotation matrices (and others).
/// They don't have singularities (Euler angles' gimbal lock), they are more compact
/// than matrices (4 floats) and can be extended to represent translations as well (dual quaternions).
/// As a set, the quaternions form \f$R^4\f$, four-dimensional vector space over the real
/// numbers (meaning they possess the usual addition and scalar multiplication). In addition,
/// quaternions can also be multiplied. Multiplication is associative, but (unlike
/// multiplication of real or complex numbers) it is not commutative. There are
/// many resources out there available covering quaternions much more extensively.
/// This particular implementation is a very minimal one: a constructor taking
/// 3D unit vector (axis of rotation) and rotation angle, addition and multiplication
/// operators, methods converting to/from 3x3 matrices and a conjugate function (non-member).
/// Example usage can be found in DualQuaternionTests.cpp file.
class Quaternion
{
public:
    Quaternion() = default;
    Quaternion(float x, float y, float z, float w): x(x), y(y), z(z), w(w) {}

    /// This constructor is enabled only when Q is not a float (assuming Q will then be
    /// a different implementation of quaternions). Q must have public x,y,z,w members.
    template <typename Q,
              typename std::enable_if<not std::is_floating_point<Q>::value>::type* = nullptr>
    Quaternion(const Q& q): x(q.x), y(q.y), z(q.z), w(q.w) {}

    /// Constructor enabled when F is a floating point. Constructs (0,0,0,f).
    /// Real numbers are a subset of quaternions (when x, y and z are zero).
    /// For example
    /// 2.f * Quaternion()
    /// is actually implemented as
    /// Quaternion(2.f) * Quaternion().
    template <typename F,
              typename std::enable_if<std::is_floating_point<F>::value>::type* = nullptr>
    Quaternion(F f): x(0.f), y(0.f), z(0.f), w(f) {}

    /// Constructs a unit quaternion from the specified unit 3D vector and rotation angle.
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

    /// Constructs a unit quaternion representing no rotation (vector component is 0,
    /// scalar is set to 1).
    static const Quaternion identity()
    {
        return Quaternion(0.f, 0.f, 0.f, 1.f);
    }

    /// Returns the vector embedded in a quaternion.
    /// Templatized to support various different implementations of 3D vectors.
    /// Example usage: Quaternion::toVector<nv::vec3f>(q).
    template <typename V3>
    static const V3 toVector(const Quaternion& q)
    {
        return V3(q.x, q.y, q.z);
    }

    /// Constructs a quaternion by embedding (identifying with) a 3D vector.
    template <typename V3>
    static const Quaternion fromVector(const V3& v)
    {
        return Quaternion(v.x, v.y, v.z, 0.f);
    }

    /// Converts a quaternion to 3x3 rotation matrix. Assumes quaternion q
    /// is unit (rotation only). Templatized on matrix type (requires
    /// an operator() (row, column)).
    template <typename M3>
    static const M3 toMatrix(const Quaternion& q)
    {
        const float x = q.x;
        const float y = q.y;
        const float z = q.z;
        const float w = q.w;
        M3 m;
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

    /// Converts a pure rotation (orthogonal) 3x3 matrix to unit quaternion.
    /// Templatized on matrix type (requires an operator() (row, column)).
    /// For more information, see e.g. Real-Time Rendering (Haines, Hoffman, Akenine-Möller).
    template <typename M3>
    static const Quaternion fromMatrix(const M3& m)
    {
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

    float x,y,z; ///< Vector part of the quaternion.

    /// Scalar part of the quaternion.
    float w;
};

/// Returns a Quaternion that is the sum of quaternions a and b (component-wise sum).
inline const Quaternion operator + (const Quaternion& a, const Quaternion& b)
{
    return Quaternion(a.x + b.x,
                      a.y + b.y,
                      a.z + b.z,
                      a.w + b.w);
}

/// Quaternion multiplication.
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
