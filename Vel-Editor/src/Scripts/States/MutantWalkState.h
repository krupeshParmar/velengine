#pragma once

#include "MutantBaseState.h"

class MutantWalkState : public MutantBaseState
{
public:
	float speed = 1.f;
	virtual void EnterState(MutantController* controller) override
	{

	}

	virtual void Update(MutantController* controller, vel::Timestep ts) override
	{
		if (controller->animatorComponent->runningAnimation && controller->animatorComponent->runningAnimation->ID != 1)
		{
			controller->animatorComponent->PlayAnimation(1);
		}

		glm::vec3 targetDir = controller->targetTransform->Translation - controller->selfTransform->Translation;
		targetDir.y = 0.f;

		targetDir = glm::normalize(targetDir);
		controller->selfTransform->Translation += targetDir * speed * ts.GetSeconds();
	}
};