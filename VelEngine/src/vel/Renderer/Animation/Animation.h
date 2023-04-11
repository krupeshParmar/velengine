#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include "BoneAnim.h"
#include <string>

class aiAnimation;
class aiNode;
class aiAnimation;

namespace vel
{
	struct AssimpNodeData
	{
		glm::mat4 transformation;
		std::string name;
		int childrenCount;
		std::vector<AssimpNodeData> children;
	};

	struct MeshData;
	struct BoneInfo;

	class Animation
	{
	public:
		Animation() = default;
		Animation(const std::string& animationPath, Ref<MeshData> mesh);
		~Animation() {}
		BoneAnim* FindBone(const std::string& name);

		inline float GetTicksPerSecond() { return m_TicksPerSecond; }

		inline float GetDuration() { return m_Duration; }

		inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

		inline const std::unordered_map<std::string, BoneInfo>* GetBoneIDMap()
		{
			return m_BoneInfoMap;
		}

		std::string name;
	private:
		void ReadMissingBones(const aiAnimation* animation, MeshData& mesh);
		void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
		float m_Duration;
		int m_TicksPerSecond;
		std::vector<BoneAnim> m_Bones;
		AssimpNodeData m_RootNode;
		std::unordered_map<std::string, BoneInfo>* m_BoneInfoMap;
	};
}