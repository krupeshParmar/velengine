#pragma once

#include "MutantBaseState.h"

class MutantRunState : public MutantBaseState
{
public:
	float speed = 4.f;
	virtual void EnterState(MutantController* controller) override
	{

	}

	virtual void Update(MutantController* controller, vel::Timestep ts) override
	{
		if (controller->animatorComponent->runningAnimation &&controller->animatorComponent->runningAnimation->ID != 2)
		{
			controller->animatorComponent->PlayAnimation(2);
		}

		glm::vec3 targetDir = controller->targetTransform->Translation - controller->selfTransform->Translation;
		targetDir.y = 0.f;
		targetDir = glm::normalize(targetDir);
		glm::quat lookRotation = glm::quatLookAt(-targetDir, glm::vec3(0.f, 1.f, 0.f));

		controller->GetComponent<vel::TransformComponent>().SetRotation(
			glm::slerp(lookRotation, controller->GetComponent<vel::TransformComponent>().GetRotation(), ts.GetSeconds())
		);

		controller->selfTransform->Translation += targetDir * speed * ts.GetSeconds();
	}
};