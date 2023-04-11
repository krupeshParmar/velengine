#pragma once

#include "PhyMath.h"

namespace physics
{
	class RigidBodyDesc
	{
	public:
		RigidBodyDesc()
			: mass(1.f)
			, isStatic(false)
			, position(0.f, 0.f, 0.f)
			, rotation(1.0f, 0.f, 0.f, 0.f)
			, linearVelocity(0.f, 0.f, 0.f)
			, angularVelocity(0.f, 0.f, 0.f)
			, linearFactor(1.f, 1.f, 1.f)
			, angularFactor(1.f, 1.f, 1.f)
			, restitution(0.6f)
			, friction(0.4f)
			, linearDamping(0.01f)
			, angularDamping(0.05f)
		{
			if (mass == 0)
				isStatic = true;
		}

		float mass;
		bool isStatic;
		float restitution;
		float friction;

		Vector3 position;
		Vector3 linearVelocity;
		float volume;

		Vector3 linearFactor;
		Vector3 angularFactor;

		Quaternion rotation;
		Vector3 angularVelocity;

		float linearDamping;
		float angularDamping;
	};
}
