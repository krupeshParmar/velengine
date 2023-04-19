#pragma once
#include "Animation.h"

namespace vel
{
	class Animator
	{
	public:
		Animator::Animator(Animation* Animation);
		~Animator();
		void Animator::UpdateAnimation(float dt);
		void Animator::PlayAnimation(Animation* pAnimation);
		void Animator::CalculateBoneTransform(const AssimpNodeData* node,const glm::mat4& parentTransform);
		std::vector<glm::mat4>& GetFinalBoneMatrices();
		const float& GetTimeStamp() { return m_CurrentTime; }

	private:
		std::vector<glm::mat4> m_FinalBoneMatrices;
		Animation* m_CurrentAnimation;
		Animation* m_PreviousAnimation;
		float m_CurrentTime;
		float m_DeltaTime;
		float m_TransitionTime = 1.f;
		unsigned int size = 400;
	};
}