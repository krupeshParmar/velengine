#pragma once

#include "MutantBaseState.h"
#include "../PlayerController.h"
#include "../MutantController.h"
#include <chrono>

class MutantAttackState : public MutantBaseState
{
public:
	bool attacked = true;
	std::chrono::steady_clock::time_point start, end;
	float elapsedTimeSinceAttack = 10.f;
	virtual void EnterState(MutantController* controller) override
	{
		start = std::chrono::steady_clock::now();
	}

	virtual void Update(MutantController* controller, vel::Timestep ts) override
	{
		end = std::chrono::steady_clock::now();

		if (std::chrono::duration_cast<std::chrono::seconds>(end - start).count() < std::chrono::seconds(1).count())
		{
			if (controller->animatorComponent->runningAnimation && controller->animatorComponent->runningAnimation->ID != 0)
			{
				controller->animatorComponent->PlayAnimation(0);
			}
			return;
		}
		float distance = glm::distance(controller->selfTransform->Translation, controller->targetTransform->Translation);

		int attackID = 3;

		if (controller->targetController->GetXP() > 70.f)
		{
			attackID = 4;
		}

		if (controller->animatorComponent->runningAnimation && 
			controller->animatorComponent->runningAnimation->ID != attackID)
		{
			if(controller->animatorComponent->runningAnimation->ID != 4 || controller->animatorComponent->runningAnimation->Finished)
				controller->animatorComponent->PlayAnimation(attackID);
		}
		if (!controller->targetController)
			return;

		if (controller->animatorComponent->runningAnimation && controller->animatorComponent->runningAnimation->ID == attackID)
		{
			float ratio = controller->animatorComponent->animator->GetTimeStamp()
				/ controller->animatorComponent->runningAnimation->GetDuration();
			if (ratio < 0.25f)
				attacked = false;
			if (ratio > 0.25f && ratio < 0.35f && !attacked && distance <= 1.5f)
			{
				if(attackID == 3)
					controller->targetController->TakeDamage(5.f);
				else
					controller->targetController->TakeDamage(25.f);
				start = std::chrono::steady_clock::now();
				attacked = true;
			}
		}
	}
};
