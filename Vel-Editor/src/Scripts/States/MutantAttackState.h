#pragma once

#include "MutantBaseState.h"

class MutantAttackState : public MutantBaseState
{
public:
	virtual void EnterState(MutantController* controller) override
	{

	}

	virtual void Update(MutantController* controller, vel::Timestep ts) override
	{
		if (controller->animatorComponent->runningAnimation && controller->animatorComponent->runningAnimation->ID != 3)
		{
			controller->animatorComponent->PlayAnimation(3);
		}
	}
};
