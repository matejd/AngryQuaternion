#ifndef __AngryDude_hpp__
#define __AngryDude_hpp__

#include "NvAppBase/NvSampleApp.h"
#include "KHR/khrplatform.h"
#include "NvGamepad/NvGamepad.h"
#include "NV/NvMath.h"
#include "NvAppBase/NvInputTransformer.h"

#include "Skinning.hpp"

class NvGLSLProgram;
class DualQuaternion;

struct MeshGL
{
    GLuint vertexBufferId;
    GLuint indexBufferId;
    GLsizei numIndices;
    GLuint albedoTextureId;
};

struct SkinnedModelGL : public SkinnedModel
{
    std::vector<MeshGL> meshesGL;
};

class AngryDudeApp : public NvSampleApp
{
public:
    AngryDudeApp(NvPlatformContext* platform);
    ~AngryDudeApp();

    virtual void initRendering() override;
    virtual void initUI() override;
    virtual void draw() override;
    virtual void reshape(int32_t width, int32_t height) override;

    virtual void configurationCallback(NvEGLConfiguration& config) override;

private:
    template <typename T> void updateSkinning();
    void getAnimatedTransform(int nodeAnimationIdx, nv::matrix4f& animatedTransform);
    void getAnimatedTransform(int nodeAnimationIdx, DualQuaternion& animatedTransform);
    nv::vec3f getInterpolatedTranslation(int nodeAnimationIdx);
    nv::quaternionf getInterpolatedRotation(int nodeAnimationIdx);

    SkinnedModelGL* mModel;
    NvGLSLProgram*  mSkinningProgram;
    NvGLSLProgram*  mDebugProgram;
    nv::matrix4f    mModelViewProjection;

    float           mTime;
    float           mTimeScalar;
    bool            mUseDQB;
    bool            mDrawSkeleton;

    int             mModelViewProjectionLocation;
    int             mBoneMatricesLocation;
    int             mBoneDualQuaternionsLocation;
    int             mUseDQBLocation;
    int             mAlbedoSampler;

    int             mPositionAttribute;
    int             mNormalAttribute;
    int             mBonesAttribute;
    int             mUVAttribute;

    int             mDebugMVPLocation;
    int             mDebugBonesLocation;
    int             mDebugPositionBoneAttr;
    GLuint          mDebugBufferId;
    int             mDebugNumIndices;
};

#endif
