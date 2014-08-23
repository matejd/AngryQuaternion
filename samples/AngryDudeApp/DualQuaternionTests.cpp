#include "DualQuaternion.hpp"
#include "NV/NvMath.h"
#include <iostream>

/// Compiled with
/// clang DualQuaternionTests.cpp -o DualQuaternionTests -g3 -Wall -std=c++11 -I. -I../../extensions/include/ -L./gtest/ -lgtest -lstdc++ -lpthread -lm
///

std::ostream& operator<<(std::ostream& os, const nv::vec3f& v)
{
    os << "(" << v.x << ", " << v.y << ", "  << v.z << ")";
    return os;
}

// Included here in order for stream << operators to be visible.
#include "gtest/gtest.h"

::testing::AssertionResult Vec3Equal(const nv::vec3f& a, const nv::vec3f& b)
{
    const float eps = 0.00001f;
    if (std::abs(a.x-b.x) < eps &&
        std::abs(a.y-b.y) < eps &&
        std::abs(a.z-b.z) < eps)
        return ::testing::AssertionSuccess();
    else
        return ::testing::AssertionFailure() << "Vectors not equal:" << a << "  and  " << b;
}

TEST(QuaternionTest, Rotation)
{
    const nv::vec3f v(-5.f, 2.f, 4.2f);
    const nv::vec3f axis = nv::normalize(nv::vec3f(1.f, -1.5f, 0.f));
    const float thetaRadians = -3.141592f / 3.f;
    ASSERT_FLOAT_EQ(nv::length(axis), 1.f);

    const Quaternion asQ = Quaternion::fromVector(v);
    const Quaternion rot(axis, thetaRadians);
    const Quaternion r = rot*asQ*conjugate(rot);

    const nv::quaternionf nq(axis, thetaRadians);
    const nv::quaternionf nv(v.x, v.y, v.z, 0.f);
    const nv::quaternionf nr = nq*nv*nv::conjugate(nq);

    EXPECT_TRUE(Vec3Equal(nv::vec3f(nr.x, nr.y, nr.z), Quaternion::toVector<nv::vec3f>(r)));
}

TEST(QuaternionTest, ToMatrix)
{
    const nv::vec3f v(-5.f, 2.f, 4.2f);
    const nv::vec3f axis = nv::normalize(nv::vec3f(1.f, -1.5f, 0.3f));
    const float thetaRadians = 3.141592f / 5.f;
    ASSERT_FLOAT_EQ(nv::length(axis), 1.f);

    const Quaternion asQ = Quaternion::fromVector(v);
    const Quaternion rot(axis, thetaRadians);
    const Quaternion r = rot*asQ*conjugate(rot);

    const nv::matrix4f nm = Quaternion::toMatrix<nv::matrix4f>(rot);
    const nv::vec3f nr = nv::vec3f(nm * nv::vec4f(v, 0.f));

    EXPECT_TRUE(Vec3Equal(nr, Quaternion::toVector<nv::vec3f>(r)));
}

TEST(QuaternionTest, FromMatrix)
{
    const nv::vec3f v(1.f, 0.42f, -4.f);
    nv::matrix4f nm;
    rotationYawPitchRoll(nm, 1.42f, 0.42f, -NV_PI / 2.f);

    const nv::vec3f nr = nv::vec3f(nm * nv::vec4f(v, 0.f));

    const Quaternion asQ = Quaternion::fromVector(v);
    const Quaternion rot = Quaternion::fromMatrix(nm);
    const Quaternion r = rot*asQ*conjugate(rot);

    //const nv::quaternionf nq(nm); // There is a bug in Nvidia's matrix-to-quaternion code!
    //std::cout <<  nq.x << " " <<  nq.y << " " <<  nq.z << " " <<  nq.w << std::endl;
    //std::cout << rot.x << " " << rot.y << " " << rot.z << " " << rot.w << std::endl;

    EXPECT_TRUE(Vec3Equal(nr, Quaternion::toVector<nv::vec3f>(r)));
}

TEST(DualQuaternionTest, Translation)
{
    // Test DualQuaternion's ability to translate vectors.
    const nv::vec3f v(-0.1f, 1.f, 1.42f);
    const nv::vec3f trans(3.14f, 4.2f, -0.42f);

    const DualQuaternion dqv = DualQuaternion::fromVector(v);
    const DualQuaternion t(trans, Quaternion::identity());
    const DualQuaternion r = t*dqv*conjugateDual(t);

    EXPECT_TRUE(Vec3Equal(v+trans, DualQuaternion::toVector<nv::vec3f>(r)));
}

TEST(DualQuaternionTest, Rotation)
{
    // Vector v is to be rotated around axis.
    const nv::vec3f v(-0.42f, 1.42f, 4.2f);
    const nv::vec3f axis = nv::normalize(nv::vec3f(1.f, 1.f, 0.f));
    const float thetaRadians = -3.141592f / 3.f;
    ASSERT_FLOAT_EQ(nv::length(axis), 1.f);

    const DualQuaternion dqv = DualQuaternion::fromVector(v);
    const DualQuaternion q(nv::vec3f(0.f, 0.f, 0.f), Quaternion(axis, thetaRadians));
    const DualQuaternion r = q*dqv*conjugateDual(q);

    // Compare with Nvidia's implementation of quaternions.
    const nv::quaternionf nq(axis, thetaRadians);
    const nv::quaternionf nv(v.x, v.y, v.z, 0.f);
    const nv::quaternionf nr = nq*nv*nv::conjugate(nq);

    EXPECT_TRUE(Vec3Equal(nv::vec3f(nr.x, nr.y, nr.z), DualQuaternion::toVector<nv::vec3f>(r)));
}

TEST(DualQuaternionTest, RotationAndTranslation)
{
    // First rotate than translate.
    const nv::vec3f v(0.42f, 1.42f, 4.2f);
    const nv::vec3f axis = nv::normalize(nv::vec3f(1.f, -1.f, 0.5f));
    const float thetaRadians = 3.141592f / 4.242f;
    const nv::vec3f trans(3.14f, 4.2f, -0.42f);
    ASSERT_FLOAT_EQ(nv::length(axis), 1.f);

    const nv::quaternionf nq(axis, thetaRadians);
    const DualQuaternion dqv = DualQuaternion::fromVector(v);
    const DualQuaternion q(trans, Quaternion(nq));
    const DualQuaternion r = q*dqv*conjugateDual(q);

    const nv::quaternionf nv(v.x, v.y, v.z, 0.f);
    const nv::quaternionf nr = nq*nv*nv::conjugate(nq);

    EXPECT_TRUE(Vec3Equal(nv::vec3f(nr.x, nr.y, nr.z) + trans, DualQuaternion::toVector<nv::vec3f>(r)));
}

TEST(DualQuaternionTest, ToMatrix)
{
    const nv::vec3f v(-5.f, 2.f, 4.2f);
    const nv::vec3f axis = nv::normalize(nv::vec3f(1.f, -1.5f, 0.3f));
    const float thetaRadians = 3.141592f / 5.f;
    ASSERT_FLOAT_EQ(nv::length(axis), 1.f);

    const nv::vec3f trans(2.f, -5.f, 0.42f);
    const DualQuaternion dqv = DualQuaternion::fromVector(v);
    const DualQuaternion q(trans, Quaternion(axis, thetaRadians));
    const DualQuaternion r = q*dqv*conjugateDual(q);

    const nv::matrix4f nm = DualQuaternion::toMatrix<nv::matrix4f>(q);
    const nv::vec3f nr = nv::vec3f(nm * nv::vec4f(v, 1.f));

    EXPECT_TRUE(Vec3Equal(nr, DualQuaternion::toVector<nv::vec3f>(r)));
}

TEST(DualQuaternionTest, FromMatrix)
{
    const nv::vec3f v(-1.f, 0.42f, 4.2f);
    const nv::vec3f trans(2.f, -5.f, 0.42f);
    nv::matrix4f nm;
    rotationYawPitchRoll(nm, 1.42f, -0.42f, -NV_PI / 2.f);
    nm.set_translate(trans);

    const nv::vec3f nr = nv::vec3f(nm * nv::vec4f(v, 1.f));

    const DualQuaternion dqv = DualQuaternion::fromVector(v);
    const DualQuaternion q = DualQuaternion::fromMatrix(nm);
    const DualQuaternion r = q*dqv*conjugateDual(q);

    EXPECT_TRUE(Vec3Equal(nr, DualQuaternion::toVector<nv::vec3f>(r)));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

