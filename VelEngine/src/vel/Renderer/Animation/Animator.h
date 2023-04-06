#pragma once
#include "Animation.h"

namespace vel
{
	class Animator
	{
	public:
		Animator::Animator(Animation* Animation);
		void Animator::UpdateAnimation(float dt);
		void Animator::PlayAnimation(Animation* pAnimation);
		void Animator::CalculateBoneTransform(const AssimpNodeData* node,const glm::mat4& parentTransform);
		std::vector<glm::mat4>& GetFinalBoneMatrices();

	private:
		std::vector<glm::mat4> m_FinalBoneMatrices;
		Animation* m_CurrentAnimation;
		float m_CurrentTime;
		float m_DeltaTime;
	};
}