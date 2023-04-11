#pragma once

#include <iSoftBody.h>
#include <SoftBodyDesc.h>
#include "RigidBody.h"

namespace physics
{
	namespace physxim
	{
		class SoftBody : public iSoftBody
		{
		public:
			SoftBody(const SoftBodyDesc& desc);
			virtual ~SoftBody(void);

			static SoftBody* Cast(iCollisionBody* body);

			virtual unsigned int GetNumNodes();
			virtual void GetNodePosition(unsigned int nodeIndex, glm::vec3& nodePositionOut);
		};
	}
}