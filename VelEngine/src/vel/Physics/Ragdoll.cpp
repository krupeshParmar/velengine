#include "velpch.h"
#include "Ragdoll.h"

#include "CylinderShape.h"
#include "ConeTwistConstraint.h"
#include "vel/Scene/Scene.h"
#include "vel/Scene/Component.h"


namespace vel
{
	Ragdoll::Ragdoll(float mass, float scale, Scene* scene,Ref<MeshData> meshData)
		: m_Mass(mass)
		, m_Scale(scale)
		, m_Scene(scene)
		, m_MeshData(meshData)
	{
		m_Bodies.resize(NUM_BODY_PARTS);
		m_Shapes.resize(NUM_BODY_PARTS);
		m_Joints.resize(NUM_JOINTS);

		Generate();

		// Hacking in the Bone Indexes for this example.
		m_BoneIndexToBodyPartIndex[0] = BodyPart_Pelvis;
		m_BoneIndexToBodyPartIndex[1] = BodyPart_Pelvis;
		m_BoneIndexToBodyPartIndex[2] = BodyPart_Spine;
		m_BoneIndexToBodyPartIndex[3] = BodyPart_Spine;
		m_BoneIndexToBodyPartIndex[4] = BodyPart_Head;
		m_BoneIndexToBodyPartIndex[5] = BodyPart_UpperLeftArm;
		m_BoneIndexToBodyPartIndex[6] = BodyPart_UpperLeftArm;
		m_BoneIndexToBodyPartIndex[7] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[8] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[9] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[10] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[11] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[12] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[13] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[14] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[15] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[16] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[17] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[18] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[19] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[20] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[21] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[22] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[23] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[24] = BodyPart_LowerLeftArm;
		m_BoneIndexToBodyPartIndex[25] = BodyPart_UpperRightArm;
		m_BoneIndexToBodyPartIndex[26] = BodyPart_UpperRightArm;
		m_BoneIndexToBodyPartIndex[27] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[28] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[29] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[30] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[31] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[32] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[33] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[34] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[35] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[36] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[37] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[38] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[39] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[40] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[41] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[42] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[43] = BodyPart_LowerRightArm;
		m_BoneIndexToBodyPartIndex[44] = BodyPart_UpperLeftLeg;
		m_BoneIndexToBodyPartIndex[45] = BodyPart_LowerLeftLeg;
		m_BoneIndexToBodyPartIndex[46] = BodyPart_LowerLeftLeg;
		m_BoneIndexToBodyPartIndex[47] = BodyPart_LowerLeftLeg;
		m_BoneIndexToBodyPartIndex[48] = BodyPart_UpperRightLeg;
		m_BoneIndexToBodyPartIndex[49] = BodyPart_LowerRightLeg;
		m_BoneIndexToBodyPartIndex[50] = BodyPart_LowerRightLeg;
		m_BoneIndexToBodyPartIndex[51] = BodyPart_LowerRightLeg;
	}

	Ragdoll::~Ragdoll()
	{

	}

	void Ragdoll::GetBodyPartTransform(int boneIndex, glm::mat4& out)
	{
		std::map<int, int>::iterator it = m_BoneIndexToBodyPartIndex.find(boneIndex);
		if (it == m_BoneIndexToBodyPartIndex.end())
			return;

		m_Bodies[it->second]->GetWorldSpaceTransform(out);
	}

	void Ragdoll::Generate()
	{
		GenerateBodyParts();
		ConnectJoints();

		if(m_Scene)
			for (int i = 0; i < NUM_JOINTS; i++)
			{
				m_Scene->GetPhysicsWorld()->AddConstraint(m_Joints[i]);
			}
	}

	void Ragdoll::GenerateBodyParts()
	{
		// Create all the shapes
		//							CreateCapsule(Radius, Height)
		m_Shapes[BodyPart_Head] = CreateCylinder(0.14f * m_Scale, 0.14f * m_Scale);
		m_Shapes[BodyPart_Spine] = CreateCylinder(0.2f * m_Scale, 0.3f * m_Scale);
		m_Shapes[BodyPart_Pelvis] = CreateCylinder(0.2f * m_Scale, 0.2f * m_Scale);
		m_Shapes[BodyPart_UpperLeftArm] = CreateCylinder(0.15f * m_Scale, 0.44f * m_Scale);
		m_Shapes[BodyPart_LowerLeftArm] = CreateCylinder(0.12f * m_Scale, 0.44f * m_Scale);
		m_Shapes[BodyPart_UpperRightArm] = CreateCylinder(0.15f * m_Scale, 0.44f * m_Scale);
		m_Shapes[BodyPart_LowerRightArm] = CreateCylinder(0.12f * m_Scale, 0.44f * m_Scale);
		m_Shapes[BodyPart_UpperLeftLeg] = CreateCylinder(0.20f * m_Scale, 0.50f * m_Scale);
		m_Shapes[BodyPart_LowerLeftLeg] = CreateCylinder(0.15f * m_Scale, 0.50f * m_Scale);
		m_Shapes[BodyPart_UpperRightLeg] = CreateCylinder(0.20f * m_Scale, 0.50f * m_Scale);
		m_Shapes[BodyPart_LowerRightLeg] = CreateCylinder(0.15f * m_Scale, 0.50f * m_Scale);

		// Create all the bodies
		m_Bodies[BodyPart_Head] = CreateBodyPart(m_Shapes[BodyPart_Head], glm::vec3(0, 3, 0), -0.14f * m_Scale);
		m_Bodies[BodyPart_Spine] = CreateBodyPart(m_Shapes[BodyPart_Spine], glm::vec3(0, 2, 0), 1.f * m_Scale);
		m_Bodies[BodyPart_Pelvis] = CreateBodyPart(m_Shapes[BodyPart_Pelvis], glm::vec3(0, 1, 0), 1.f * m_Scale);
		m_Bodies[BodyPart_UpperLeftArm] = CreateBodyPart(m_Shapes[BodyPart_UpperLeftArm], glm::vec3(-1, 2, 0), 0.4f * m_Scale);
		m_Bodies[BodyPart_LowerLeftArm] = CreateBodyPart(m_Shapes[BodyPart_LowerLeftArm], glm::vec3(-1, 1, 0), 0.2f * m_Scale);
		m_Bodies[BodyPart_UpperRightArm] = CreateBodyPart(m_Shapes[BodyPart_UpperRightArm], glm::vec3(1, 2, 0), 0.4f * m_Scale);
		m_Bodies[BodyPart_LowerRightArm] = CreateBodyPart(m_Shapes[BodyPart_LowerRightArm], glm::vec3(1, 1, 0), 0.2f * m_Scale);
		m_Bodies[BodyPart_UpperLeftLeg] = CreateBodyPart(m_Shapes[BodyPart_UpperLeftLeg], glm::vec3(-1, 1, 0), 0.6f * m_Scale);
		m_Bodies[BodyPart_LowerLeftLeg] = CreateBodyPart(m_Shapes[BodyPart_LowerLeftLeg], glm::vec3(-1, 0, 0), 0.3f * m_Scale);
		m_Bodies[BodyPart_UpperRightLeg] = CreateBodyPart(m_Shapes[BodyPart_UpperRightLeg], glm::vec3(1, 1, 0), 0.6f * m_Scale);
		m_Bodies[BodyPart_LowerRightLeg] = CreateBodyPart(m_Shapes[BodyPart_LowerRightLeg], glm::vec3(1, 0, 0), 0.3f * m_Scale);

	}

	void Ragdoll::ConnectJoints()
	{
		CreateJoint(
			Joint_Neck,
			BodyPart_Head,
			BodyPart_Spine,
			physics::Vector3(0.f, -0.24f, 0.f) * m_Scale,
			physics::Vector3(0.f, 0.38f, 0.f) * m_Scale
		);

		CreateJoint(
			Joint_Pelvis,
			BodyPart_Spine,
			BodyPart_Pelvis,
			physics::Vector3(0.f, -0.35, 0.f) * m_Scale,
			physics::Vector3(0.f, 0.33f, 0.f) * m_Scale
		);

		CreateJoint(
			Joint_LeftHip,
			BodyPart_Pelvis,
			BodyPart_UpperLeftLeg,
			physics::Vector3(-0.2f, -0.33f, 0.f) * m_Scale,
			physics::Vector3(0.0f, 0.48f, 0.f) * m_Scale
		);

		CreateJoint(
			Joint_LeftKnee,
			BodyPart_UpperLeftLeg,
			BodyPart_LowerLeftLeg,
			physics::Vector3(0.0f, -0.48f, 0.f) * m_Scale,
			physics::Vector3(0.0f, 0.43f, 0.f) * m_Scale
		);

		CreateJoint(
			Joint_RightHip,
			BodyPart_Pelvis,
			BodyPart_UpperRightLeg,
			physics::Vector3(0.2f, -0.33f, 0.f) * m_Scale,
			physics::Vector3(0.0f, 0.48f, 0.f) * m_Scale
		);

		CreateJoint(
			Joint_RightKnee,
			BodyPart_UpperRightLeg,
			BodyPart_LowerRightLeg,
			physics::Vector3(0.0f, -0.48f, 0.f) * m_Scale,
			physics::Vector3(0.0f, 0.43f, 0.f) * m_Scale
		);

		CreateJoint(
			Joint_LeftShoulder,
			BodyPart_Spine,
			BodyPart_UpperLeftArm,
			physics::Vector3(-0.28f, 0.28f, 0) * m_Scale,
			physics::Vector3(0, 0.4, 0) * m_Scale
		);

		CreateJoint(
			Joint_LeftElbow,
			BodyPart_UpperLeftArm,
			BodyPart_LowerLeftArm,
			physics::Vector3(0, -0.4f, 0) * m_Scale,
			physics::Vector3(0, 0.37, 0) * m_Scale
		);

		CreateJoint(
			Joint_RightShoulder,
			BodyPart_Spine,
			BodyPart_UpperRightArm,
			physics::Vector3(0.28f, 0.28f, 0) * m_Scale,
			physics::Vector3(0, 0.4, 0) * m_Scale
		);

		CreateJoint(
			Joint_RightElbow,
			BodyPart_UpperRightArm,
			BodyPart_LowerRightArm,
			physics::Vector3(0, -0.4f, 0) * m_Scale,
			physics::Vector3(0, 0.37, 0) * m_Scale
		);

	}

	physics::iRigidBody* Ragdoll::CreateBodyPart(physics::iShape* capsule, const physics::Vector3& initialOffset, float mass)
	{
		physics::RigidBodyDesc desc;
		desc.mass = mass < 0 ? 0 : mass;
		desc.position = initialOffset;
		//if (mass < 0)
		//{
		//	desc.isStatic = true;
		//	desc.position.y = 4.0f;
		//}
		return m_Scene->GetPhysicsFactory()->CreateRigidBody(desc, capsule);
	}

	physics::iShape* Ragdoll::CreateCylinder(float radius, float height)
	{
		return new physics::CylinderShape(radius, height);
	}

	void Ragdoll::CreateJoint(Joint joint, BodyPart bodyPartA, BodyPart bodyPartB, physics::Vector3 offsetA, physics::Vector3 offsetB)
	{
		glm::mat4 transformA = glm::translate(glm::mat4(1.f), offsetA);
		glm::mat4 transformB = glm::translate(glm::mat4(1.f), offsetB);
		m_Joints[joint] = new physics::ConeTwistConstraint(m_Bodies[bodyPartA], m_Bodies[bodyPartB], transformA, transformB);
	}
}