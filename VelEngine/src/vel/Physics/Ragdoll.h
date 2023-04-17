#pragma once
#pragma once

#include <vector>
#include <map>

#include "iRigidBody.h"
#include "iConstraint.h"
#include "iShape.h"
#include "Vel.h"

namespace vel
{
	class Scene;
	struct MeshData;
	enum BodyPart
	{
		BodyPart_Pelvis,
		BodyPart_Spine,
		BodyPart_Head,
		BodyPart_UpperLeftArm,
		BodyPart_LowerLeftArm,
		BodyPart_UpperRightArm,
		BodyPart_LowerRightArm,
		BodyPart_UpperLeftLeg,
		BodyPart_LowerLeftLeg,
		BodyPart_UpperRightLeg,
		BodyPart_LowerRightLeg,
		NUM_BODY_PARTS
	};

	class Ragdoll
	{
	private:

		enum Joint
		{
			Joint_Pelvis,
			Joint_Neck,
			Joint_LeftShoulder,
			Joint_LeftElbow,
			Joint_RightShoulder,
			Joint_RightElbow,
			Joint_LeftHip,
			Joint_LeftKnee,
			Joint_RightHip,
			Joint_RightKnee,
			NUM_JOINTS
		};

		typedef struct BodyPartInfo
		{
			float width;
			float height;
			float mass;
			physics::Vector3 offset;		/* pivot at center/middle */
		} BodyPartInfo;

	public:
		Ragdoll(float mass, float scale, Scene* scene, Ref<MeshData> meshData);
		~Ragdoll();

		void SetScene(Scene* scene)
		{
			this->m_Scene = scene;
		}

		physics::iRigidBody* GetBodyPart(BodyPart part) {
			return m_Bodies[part];
		}
		void GetBodyPartTransform(int boneIndex, glm::mat4& out);

	private:
		void Generate();
		void GenerateBodyParts();
		void ConnectJoints();

		physics::iRigidBody* CreateBodyPart(physics::iShape* capsule, const physics::Vector3& initialOffset, float mass);
		physics::iShape* CreateCylinder(float radius, float height);
		void CreateJoint(Joint joint, BodyPart bodyPartA, BodyPart bodyPartB, physics::Vector3 offsetA, physics::Vector3 offsetB);

	private:
		std::vector<physics::iShape*> m_Shapes;
		std::vector<physics::iConstraint*> m_Joints;
		std::vector<physics::iRigidBody*> m_Bodies;
		std::vector<BodyPartInfo> m_BodyPartInfo;
		std::map<int, int> m_BoneIndexToBodyPartIndex;

		Scene* m_Scene = nullptr; 
		Ref<MeshData> m_MeshData;

		float m_Mass;
		float m_Scale;
	};
}