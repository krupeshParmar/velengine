#pragma once
#include "../MutantController.h"

class MutantBaseState
{
public:

	virtual void EnterState(MutantController* controller) = 0;

	virtual void Update(MutantController* controller, vel::Timestep ts) = 0;
};