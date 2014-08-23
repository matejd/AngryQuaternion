#ifndef __Skinning_hpp__
#define __Skinning_hpp__

#include "NV/NvMath.h"

#include <vector>
#include <string>

struct Vertex
{
    nv::vec3f position;
    nv::vec3f normal;
    nv::vec4f bones;
    nv::vec2f uv;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;
    std::string albedoTextureFilename;
};

struct AnimationKey
{
    nv::vec4f value;
    float time;
};

struct NodeAnimation
{
    std::vector<AnimationKey> translationKeys;
    std::vector<AnimationKey> rotationKeys;
};

struct ModelNode
{
    std::string name;
    std::vector<int> childrenIndices;
    nv::matrix4f defaultTransform;
    int nodeAnimationIdx;
    int boneIdx;
};

struct Bone
{
    nv::matrix4f offset;
};

struct SkinnedModel
{
    std::vector<Mesh> meshes;
    std::vector<NodeAnimation> nodeAnimations;
    std::vector<ModelNode> modelNodes;
    std::vector<Bone> bones;
};


/// Cereal's external serialization templates.
/// These were placed in the cereal namespace in
/// order to be properly visible.
namespace cereal {

template<class Archive> void serialize(Archive& archive, nv::vec2f& v)
{
    archive(v.x, v.y);
}

template<class Archive> void serialize(Archive& archive, nv::vec3f& v)
{
    archive(v.x, v.y, v.z);
}

template<class Archive> void serialize(Archive& archive, nv::vec4f& v)
{
    archive(v.x, v.y, v.z, v.w);
}

template<class Archive> void serialize(Archive& archive, nv::matrix4f& v)
{
    archive(v._11, v._12, v._13, v._14,
            v._21, v._22, v._23, v._24,
            v._31, v._32, v._33, v._34,
            v._41, v._42, v._43, v._44);
}

template<class Archive> void serialize(Archive& archive, Vertex& v)
{
    archive(v.position, v.normal, v.bones, v.uv);
}

template<class Archive> void serialize(Archive& archive, Mesh& mesh)
{
    archive(mesh.vertices, mesh.indices, mesh.albedoTextureFilename);
}

template<class Archive> void serialize(Archive& archive, AnimationKey& key)
{
    archive(key.value, key.time);
}

template<class Archive> void serialize(Archive& archive, NodeAnimation& animation)
{
    archive(animation.translationKeys, animation.rotationKeys);
}

template<class Archive> void serialize(Archive& archive, ModelNode& node)
{
    archive(node.name, node.childrenIndices, node.defaultTransform, node.nodeAnimationIdx, node.boneIdx);
}

template<class Archive> void serialize(Archive& archive, Bone& bone)
{
    archive(bone.offset);
}

template<class Archive> void serialize(Archive& archive, SkinnedModel& model)
{
    archive(model.meshes, model.nodeAnimations, model.modelNodes, model.bones);
}

} // namespace cereal

#endif
