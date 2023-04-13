#include "velpch.h"
#include "Animation.h"
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/anim.h>
#include <vel/scene/Component.h>
#include <assimp/scene.h>
#include "vel/Math/AssimpGLMHelpers.h"

namespace vel
{
	Animation::Animation(const std::string& animationPath, Ref<MeshData> mesh)
	{
		m_Bones.reserve(100);
		m_BoneInfoMap = new std::unordered_map<std::string, BoneInfo>();
		m_BoneInfoMap->reserve(100);
		unsigned int Flags = aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices;
		Assimp::Importer* importer = new Assimp::Importer();
		const aiScene* scene = importer->ReadFile(animationPath.c_str(), 0);
		VEL_CORE_ERROR(importer->GetErrorString());
		VEL_CORE_ASSERT(scene && scene->mRootNode, "");
		auto animation = scene->mAnimations[0];
		animation->mNumChannels;
		name = animationPath;
		m_Duration = animation->mDuration;
		m_TicksPerSecond = animation->mTicksPerSecond;
		ReadHeirarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *mesh);
	}
	BoneAnim* Animation::FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const BoneAnim& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}
	void Animation::ReadMissingBones(const aiAnimation* animation, MeshData& mesh)
	{
		int size = animation->mNumChannels;
		VEL_CORE_INFO("Animation {0} has {1} channels", animation->mName.C_Str(), size);

		std::unordered_map<std::string, BoneInfo> boneInfoMap = *mesh.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
		int& boneCount = mesh.GetBoneCount(); //getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(BoneAnim(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));
		}

		*m_BoneInfoMap = boneInfoMap;
	}
	void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		VEL_CORE_ASSERT(src, "");
		dest.name = src->mName.data;
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
		dest.childrenCount = src->mNumChildren;
		dest.children.reserve(src->mNumChildren);
		for (int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHeirarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
}