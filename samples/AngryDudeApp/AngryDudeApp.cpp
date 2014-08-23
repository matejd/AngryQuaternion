#include "AngryDudeApp.hpp"

#include "NvUI/NvTweakBar.h"
#include "NvAppBase/NvFramerateCounter.h"
#include "NV/NvStopWatch.h"
#include "NvAssetLoader/NvAssetLoader.h"
#include "NvGLUtils/NvGLSLProgram.h"
#include "NvGLUtils/NvImage.h"
#include "NV/NvLogs.h"

#include "Skinning.hpp"
#include "DualQuaternion.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

#include <fstream>
#include <utility>
#include <cstddef>

void AngryDudeApp::draw()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearDepthf(1.f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glActiveTexture(GL_TEXTURE0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float fov   = 45.0f;
    const float ratio = static_cast<GLfloat>(m_width) / m_height;
    nv::perspective(mModelViewProjection, fov, ratio, 0.1f, 100.0f);
    nv::matrix4f scale;
    scale.set_scale(nv::vec3f(0.3f, 0.3f, 0.3f));
    mModelViewProjection *= m_transformer->getModelViewMat();
    mModelViewProjection *= scale;
    mSkinningProgram->enable();
    mSkinningProgram->setUniformMatrix4fv(mModelViewProjectionLocation, mModelViewProjection._array, 1, false);
    mSkinningProgram->setUniform1i(mUseDQBLocation, mUseDQB);
    mSkinningProgram->disable();

    if (mUseDQB)
        updateSkinning<DualQuaternion>();
    else
        updateSkinning<nv::matrix4f>();

    mSkinningProgram->enable();
    glEnableVertexAttribArray(mPositionAttribute);
    glEnableVertexAttribArray(mNormalAttribute);
    glEnableVertexAttribArray(mBonesAttribute);
    glEnableVertexAttribArray(mUVAttribute);

    for (const MeshGL& mesh: mModel->meshesGL) {
        mSkinningProgram->bindTexture2D(mAlbedoSampler, 0, mesh.albedoTextureId);
        glBindBuffer(GL_ARRAY_BUFFER,         mesh.vertexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferId);

        #define ATTR_OFFSET(type, member) reinterpret_cast<GLvoid*>(offsetof(type, member))
        glVertexAttribPointer(mPositionAttribute, 3, GL_FLOAT, false, sizeof(Vertex), ATTR_OFFSET(Vertex, position));
        glVertexAttribPointer(mNormalAttribute,   3, GL_FLOAT, false, sizeof(Vertex), ATTR_OFFSET(Vertex, normal));
        glVertexAttribPointer(mBonesAttribute,    4, GL_FLOAT, false, sizeof(Vertex), ATTR_OFFSET(Vertex, bones));
        glVertexAttribPointer(mUVAttribute,       2, GL_FLOAT, false, sizeof(Vertex), ATTR_OFFSET(Vertex, uv));
        #undef ATTR_OFFSET

        glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_SHORT, 0);
        CHECK_GL_ERROR();
    }

    glDisableVertexAttribArray(mPositionAttribute);
    glDisableVertexAttribArray(mNormalAttribute);
    glDisableVertexAttribArray(mBonesAttribute);
    glDisableVertexAttribArray(mUVAttribute);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    mSkinningProgram->disable();

    if (mDrawSkeleton) {
        glDisable(GL_DEPTH_TEST);
        mDebugProgram->enable();
        mDebugProgram->setUniformMatrix4fv(mDebugMVPLocation, mModelViewProjection._array, 1, false);
        glBindBuffer(GL_ARRAY_BUFFER, mDebugBufferId);
        glEnableVertexAttribArray(mDebugPositionBoneAttr);
        glVertexAttribPointer(mDebugPositionBoneAttr, 4, GL_FLOAT, false, 4*sizeof(float), 0);
        glDrawArrays(GL_LINES, 0, mDebugNumIndices);
        glDisableVertexAttribArray(mDebugPositionBoneAttr);
        mDebugProgram->disable();
        CHECK_GL_ERROR();
    }
}

nv::vec3f AngryDudeApp::getInterpolatedTranslation(int nodeAnimationIdx)
{
    const NodeAnimation& anim = mModel->nodeAnimations[nodeAnimationIdx];
    size_t index0 = 0;
    size_t index1 = 0;
    while (index1 < anim.translationKeys.size() && anim.translationKeys[index1].time < mTime)
        index1++;
    if (index1 == anim.translationKeys.size())
        index1 = 0;
    if (index1 > 0)
        index0 = index1-1;

    const nv::vec4f& trans0 = anim.translationKeys[index0].value;
    const nv::vec4f& trans1 = anim.translationKeys[index1].value;
    const nv::vec3f  trans03 = nv::vec3f(trans0.x, trans0.y, trans0.z);
    const nv::vec3f  trans13 = nv::vec3f(trans1.x, trans1.y, trans1.z);

    const float t = (mTime - anim.translationKeys[index0].time) /
                    (anim.translationKeys[index1].time - anim.translationKeys[index0].time);
    return (1.f-t)*trans03 + t*trans13;
}

nv::quaternionf AngryDudeApp::getInterpolatedRotation(int nodeAnimationIdx)
{
    const NodeAnimation& anim = mModel->nodeAnimations[nodeAnimationIdx];
    size_t index0 = 0;
    size_t index1 = 0;
    while (index1 < anim.rotationKeys.size() && anim.rotationKeys[index1].time < mTime)
        index1++;
    if (index1 == anim.rotationKeys.size())
        index1 = 0;
    if (index1 > 0)
        index0 = index1-1;

    const nv::vec4f& rot0 = anim.rotationKeys[index0].value;
    const nv::vec4f& rot1 = anim.rotationKeys[index1].value;
    const nv::quaternionf rotq0 = nv::quaternionf(rot0.x, rot0.y, rot0.z, rot0.w);
    const nv::quaternionf rotq1 = nv::quaternionf(rot1.x, rot1.y, rot1.z, rot1.w);

    const float t = (mTime - anim.rotationKeys[index0].time) /
                    (anim.rotationKeys[index1].time - anim.rotationKeys[index0].time);
    return nv::slerp(rotq0, rotq1, t);
}

void AngryDudeApp::getAnimatedTransform(int nodeAnimationIdx, nv::matrix4f& animatedTransform)
{
    const nv::vec3f translation = getInterpolatedTranslation(nodeAnimationIdx);
    const nv::quaternionf rotation = getInterpolatedRotation(nodeAnimationIdx);
    rotation.get_value(animatedTransform);
    animatedTransform.set_translate(translation);
}

void AngryDudeApp::getAnimatedTransform(int nodeAnimationIdx, DualQuaternion& animatedTransform)
{
    const nv::vec3f translation = getInterpolatedTranslation(nodeAnimationIdx);
    const nv::quaternionf rotation = getInterpolatedRotation(nodeAnimationIdx);
    animatedTransform = DualQuaternion(translation, rotation);
}

// We cannot overload on return type only, but we *can* selectively
// remove functions from overload resolution.
template <typename T>
typename std::enable_if<std::is_same<T, DualQuaternion>::value, T>::type toT(const nv::matrix4f& m)
{
    return DualQuaternion::fromMatrix(m);
}

template <typename T>
typename std::enable_if<std::is_same<T, nv::matrix4f>::value, T>::type toT(const nv::matrix4f& m)
{
    return m;
}

template <typename T>
typename std::enable_if<std::is_same<T, nv::matrix4f>::value, T>::type toT(const DualQuaternion& dq)
{
    return DualQuaternion::toMatrix<nv::matrix4f>(dq);
}

nv::matrix4f translation(const nv::vec3f& t)
{
    nv::matrix4f m;
    m(0,3) = t.x;
    m(1,3) = t.y;
    m(2,3) = t.z;
    return m;
}

nv::matrix4f identity()
{
    return nv::matrix4f();
}

// updateSkinning is templatized (T being either nv::matrix4f or DualQuaternion)
// in order to avoid code duplication. The only difference between matrix and dual
// quaternion approaches (in this implementation) is in the mathematical object
// used to represent rigid body transformations. Since we store bone offsets
// with matrices, we have to convert those to dual quaternions (toT<T>())
// when skinning with dual quaternions.
// updateSkinning goes through the animated body's node hieararchy (nodes are usually
// bones, but some nodes do not have a corresponding bone, see Assimp's documentation
// for more info). For each node in the hierarchy, we compute cumulative
// transform from the root node.
template <typename T>
void AngryDudeApp::updateSkinning()
{
    mTime += mTimeScalar * getFrameDeltaTime();
    const float animationDuration = 1.26f;
    if (mTime > animationDuration)
        mTime = mTime - animationDuration;

    T boneTransformArray[60];
    nv::matrix4f debugTransforms[60];
    assert(60 > mModel->bones.size());
    const T rootInverse = toT<T>(translation(nv::vec3f(0.f, -30.f, 0.f)));

    typedef std::pair<int, T> NodeIdxCumulativeTransform;
    std::vector<NodeIdxCumulativeTransform> breadth{std::make_pair(0, toT<T>(identity()))};

    while (!breadth.empty()) {
        std::vector<NodeIdxCumulativeTransform> children;

        for (const NodeIdxCumulativeTransform& nct: breadth) {
            const ModelNode& node = mModel->modelNodes[nct.first];
            T nodeTransform = toT<T>(node.defaultTransform);
            if (node.nodeAnimationIdx != -1) {
                getAnimatedTransform(node.nodeAnimationIdx, nodeTransform);
            }

            const T& parentCumulativeTransform = nct.second;
            const T cumulativeTransform = parentCumulativeTransform * nodeTransform;

            if (node.boneIdx != -1) {
                const Bone& bone = mModel->bones[node.boneIdx];
                boneTransformArray[node.boneIdx] = rootInverse * cumulativeTransform * toT<T>(bone.offset);
                debugTransforms[node.boneIdx] = toT<nv::matrix4f>(rootInverse * cumulativeTransform);
            }

            for (int childIndex: node.childrenIndices) {
                children.push_back(std::make_pair(childIndex, cumulativeTransform));
            }
        }

        breadth = children;
    }

    mSkinningProgram->enable();
    if (std::is_same<T, DualQuaternion>::value)
        mSkinningProgram->setUniform4fv(mBoneDualQuaternionsLocation, reinterpret_cast<float*>(&boneTransformArray[0]), mModel->bones.size()*2);
    else
        mSkinningProgram->setUniformMatrix4fv(mBoneMatricesLocation, reinterpret_cast<float*>(&boneTransformArray[0]), mModel->bones.size(), false);
    mSkinningProgram->disable();

    mDebugProgram->enable();
    mDebugProgram->setUniformMatrix4fv(mDebugBonesLocation, reinterpret_cast<float*>(&debugTransforms[0]), mModel->bones.size(), false);
    mDebugProgram->disable();
}

void AngryDudeApp::initRendering() {
    NvImage::UpperLeftOrigin(false);
    NvAssetLoaderAddSearchPath("AngryDudeApp");
    mSkinningProgram = NvGLSLProgram::createFromFiles("skinning.vert", "diffuse.frag");
    mDebugProgram    = NvGLSLProgram::createFromFiles("debug.vert", "debug.frag");
    mDebugMVPLocation = mDebugProgram->getUniformLocation("mvp");
    mDebugBonesLocation = mDebugProgram->getUniformLocation("boneMatrices");
    mDebugPositionBoneAttr = mDebugProgram->getAttribLocation("positionBone");

    mModelViewProjectionLocation = mSkinningProgram->getUniformLocation("mvp");
    mBoneMatricesLocation        = mSkinningProgram->getUniformLocation("boneMatrices");
    mBoneDualQuaternionsLocation = mSkinningProgram->getUniformLocation("boneDualQuaternions");
    mUseDQBLocation              = mSkinningProgram->getUniformLocation("useDQB");
    mAlbedoSampler               = mSkinningProgram->getUniformLocation("sampler0");
    mPositionAttribute = mSkinningProgram->getAttribLocation("position");
    mNormalAttribute   = mSkinningProgram->getAttribLocation("normal");
    mBonesAttribute    = mSkinningProgram->getAttribLocation("bones");
    mUVAttribute       = mSkinningProgram->getAttribLocation("uv");

    m_transformer->setRotationVec(nv::vec3f(0.0f, NV_PI*0.25f, 0.0f));
    m_transformer->setTranslationVec(nv::vec3f(0.0f, 0.0f, -25.0f));
    m_transformer->setMaxTranslationVel(50.0f);

    CHECK_GL_ERROR();

    struct memorybuf : public std::streambuf {
        memorybuf(char* p, std::size_t n) {
            setg(p, p, p+n);
        }
    };

    int numBytes = 0;
    char* pdude = NvAssetLoaderRead("dude.binmesh", numBytes);
    assert(numBytes > 0);
    memorybuf mb(pdude, numBytes);
    std::istream is(&mb);
    cereal::BinaryInputArchive iarchive(is);
    mModel = new SkinnedModelGL;
    iarchive(*mModel);
    NvAssetLoaderFree(pdude);

    for (const Mesh& mesh: mModel->meshes) {
        MeshGL meshGL;
        meshGL.numIndices = mesh.indices.size();
        glGenBuffers(1, &meshGL.indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshGL.indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(mesh.indices[0]),
                                              mesh.indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glGenBuffers(1, &meshGL.vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, meshGL.vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(mesh.vertices[0]),
                                      mesh.vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        meshGL.albedoTextureId = NvImage::UploadTextureFromDDSFile(mesh.albedoTextureFilename.c_str());
        glBindTexture(GL_TEXTURE_2D, meshGL.albedoTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        mModel->meshesGL.push_back(meshGL);
    }

    // Build debug skeleton.
    std::vector<nv::vec4f> debugLines;
    for (const ModelNode& mn: mModel->modelNodes) {
        for (int childIdx: mn.childrenIndices) {
            const int parentBoneIdx = mn.boneIdx;
            const int childBoneIdx = mModel->modelNodes[childIdx].boneIdx;
            if (parentBoneIdx == -1 || childBoneIdx == -1)
                continue;
            const nv::vec4f startPositionBone = nv::vec4f(0.f, 0.f, 0.f, static_cast<float>(parentBoneIdx));
            const nv::vec4f endPositionBone   = nv::vec4f(0.f, 0.f, 0.f, static_cast<float>(childBoneIdx));
            debugLines.push_back(startPositionBone);
            debugLines.push_back(endPositionBone);
        }
    }
    mDebugNumIndices = debugLines.size();
    glGenBuffers(1, &mDebugBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, mDebugBufferId);
    glBufferData(GL_ARRAY_BUFFER, debugLines.size() * sizeof(debugLines[0]), debugLines.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERROR();
}

AngryDudeApp::AngryDudeApp(NvPlatformContext* platform)
    : NvSampleApp(platform, "Angry Dude")
    , mModel(nullptr)
    , mSkinningProgram(nullptr)
    , mTimeScalar(0.1f)
    , mUseDQB(true)
    , mDrawSkeleton(false)
    , mTime(0.f)
{
    // Required in all subclasses to avoid silent link issues.
    forceLinkHack();
}

AngryDudeApp::~AngryDudeApp()
{
    delete mModel;
    delete mSkinningProgram;
    delete mDebugProgram;
    NvAssetLoaderShutdown();
    LOGI("AngryDudeApp: destroyed\n");
}

void AngryDudeApp::initUI()
{
    if (mTweakBar) {
        NvTweakVarBase* var = mTweakBar->addValue("Animation Speed", mTimeScalar, 0, 5.0, 0.01f);
        addTweakKeyBind(var, NvKey::K_RBRACKET, NvKey::K_LBRACKET);
        addTweakButtonBind(var, NvGamepad::BUTTON_RIGHT_SHOULDER, NvGamepad::BUTTON_LEFT_SHOULDER);

        mTweakBar->addPadding();
        var = mTweakBar->addValue("Dual Quaternion Blending", mUseDQB);
        addTweakKeyBind(var, NvKey::K_B);
        addTweakButtonBind(var, NvGamepad::BUTTON_X);

        mTweakBar->addPadding();
        var = mTweakBar->addValue("Draw Skeleton", mDrawSkeleton);
        addTweakKeyBind(var, NvKey::K_N);
    }

    mFramerate->setMaxReportRate(.2f);
    mFramerate->setReportFrames(20);
    // Disable wait for vsync.
    getGLContext()->setSwapInterval(0);
}

void AngryDudeApp::reshape(int32_t width, int32_t height)
{
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));
    CHECK_GL_ERROR();
}

void AngryDudeApp::configurationCallback(NvEGLConfiguration& config)
{
    config.depthBits = 24;
    config.stencilBits = 0;
    config.apiVer = NvGfxAPIVersionES2();
}

NvAppBase* NvAppFactory(NvPlatformContext* platform)
{
    return new AngryDudeApp(platform);
}
