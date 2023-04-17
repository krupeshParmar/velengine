#pragma once

#include "MutantBaseState.h"

class MutantIdleState : public MutantBaseState
{

	virtual void EnterState(MutantController* controller) override
	{

	}

	virtual void Update(MutantController* controller, vel::Timestep ts) override
	{
		if (!controller->animatorComponent->runningAnimation)
		{
			controller->animatorComponent->PlayAnimation(0);
		}
		if (controller->animatorComponent->runningAnimation && controller->animatorComponent->runningAnimation->ID != 0)
		{
			controller->animatorComponent->PlayAnimation(0);
		}
	}
};