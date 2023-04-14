#include "velpch.h"
#include "Animator.h"
#include "vel/Scene/Component.h"
#include <glm/gtx/quaternion.hpp>

namespace vel
{
	Animator::Animator(Animation* currentAnimation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = currentAnimation;
		m_PreviousAnimation = nullptr;
		unsigned int size = 300;
		m_FinalBoneMatrices.reserve(size);

		for (int i = 0; i < size; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}
	void Animator::UpdateAnimation(float dt)
	{
		m_DeltaTime = dt; 
		if (m_TransitionTime > 0.0f)
		{
			m_TransitionTime -= dt;
			if (m_TransitionTime < 0.0f)
			{
				m_TransitionTime = 0.0f;
			}
		}
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * m_CurrentAnimation->GetSpeed() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}
	void Animator::PlayAnimation(Animation* pAnimation)
	{
		m_PreviousAnimation = m_CurrentAnimation;
		m_TransitionTime = pAnimation->SpeedAndTransitionTime.y;
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}
	void Animator::CalculateBoneTransform(const AssimpNodeData* node,const glm::mat4& parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		BoneAnim* Bone = m_CurrentAnimation->FindBone(nodeName);
		BoneAnim* bone2 = nullptr;
		if(m_PreviousAnimation)
			bone2 = m_PreviousAnimation->FindBone(nodeName);
		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
			if (bone2 && m_TransitionTime > 0.f)
			{
				glm::vec3 translation, scale;
				glm::quat rotation;
				Math::DecomposeTransform(nodeTransform, translation, rotation, scale);
				glm::mat4 prevTransform = bone2->GetLocalTransform();
				glm::vec3 translation2, scale2;
				glm::quat rotation2;
				Math::DecomposeTransform(prevTransform, translation2, rotation2, scale2);

				float curRatio = 1.f - m_TransitionTime;
				float prevRatio = m_TransitionTime;
				translation = translation * curRatio + translation2 * prevRatio;
				scale = scale * curRatio + scale2 * prevRatio;
				rotation = glm::slerp(rotation2, rotation, curRatio);
				nodeTransform = glm::translate(glm::mat4(1.f), translation)
					* glm::toMat4(rotation)
					* glm::scale(glm::mat4(1.f), scale);
			}
		}
		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap->find(nodeName) != boneInfoMap->end())
		{
			int index = boneInfoMap->at(nodeName).id;
			glm::mat4 offset = boneInfoMap->at(nodeName).offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}
	std::vector<glm::mat4>& Animator::GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}
}