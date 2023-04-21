#include "pch.h"
#include "PhysicsWorld.h"
#include <BoxShape.h>
#include <CylinderShape.h>
#include "CharacterController.h"
#include <characterkinematic/PxController.h>
#include <iostream>
#include <lowlevel/NvBlast.h>
#include <ConeTwistConstraint.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <DistanceConstraint.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
CRITICAL_SECTION JOINTS_LOCK;
CRITICAL_SECTION UPDATE_LOCK;

namespace physics
{
	namespace physxim
	{
		PhysicsWorld::PhysicsWorld()
			: iPhysicsWorld()
			, m_CollisionHandler(nullptr)
		{
			InitializeCriticalSection(&JOINTS_LOCK);
			InitializeCriticalSection(&UPDATE_LOCK);
			static physx::PxDefaultErrorCallback gDefaultErrorCallback;
			static physx::PxDefaultAllocator gDefaultAllocatorCallback;

			mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
				gDefaultErrorCallback);
			if (!mFoundation)
				throw("PxCreateFoundation failed!");

			mPVD = PxCreatePvd(*mFoundation);
			physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
			mPVD->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

			mToleranceScale.length = 100;
			mToleranceScale.speed = 981;

			mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPVD);
			if (!mPhysics)
				throw("PxCreatePhysics failed!");

			physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
			sceneDesc.gravity = physx::PxVec3(0.f, -9.81f, 0.f);
			mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
			sceneDesc.cpuDispatcher = mDispatcher;
			sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
			mScene = mPhysics->createScene(sceneDesc);

			physx::PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
			if (pvdClient)
			{
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
			}
			m_CollisionHandler = new CollisionHandler();
			mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);

			mControllerManager = PxCreateControllerManager(*mScene);

			physx::PxRigidActor* groundPlane = physx::PxCreatePlane(
				*mPhysics, physx::PxPlane(
					0.f,
					1.f,
					0.f,
					0.5f
				), *mMaterial);
			if (groundPlane)
			{
				mScene->addActor(*groundPlane);
				groundPlane->userData = nullptr;
				m_RigidBodies.push_back(groundPlane);
			}
			/*mScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
			mScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);
			mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, 1.0f);
			mScene->setVisualizationParameter(physx::PxVisualizationParameter::eWORLD_AXES, 1.0f);*/
			
			/*physx::PxRigidActor* sidePlane = physx::PxCreatePlane(
				*mPhysics, physx::PxPlane(
					1.f,
					0.f,
					0.f,
					-30.f
				), *mMaterial);
			if (sidePlane)
			{
				mScene->addActor(*sidePlane);
				sidePlane->userData = nullptr;
				m_RigidBodies.push_back(sidePlane);
			}*/
		}

		PhysicsWorld::~PhysicsWorld()
		{
			DeleteCriticalSection(&JOINTS_LOCK);
			DeleteCriticalSection(&UPDATE_LOCK);
			for (physx::PxRigidActor* rigidActor : m_RigidBodies)
				rigidActor->release();
			m_RigidBodies.clear();

			for (physx::PxJoint* joint : m_Joints)
				joint->release();
			m_Joints.clear();

			PX_RELEASE(mScene);
			PX_RELEASE(mDispatcher);
			PX_RELEASE(mPhysics);
			if (mPVD)
			{
				physx::PxPvdTransport* transport = mPVD->getTransport();
				mPVD->release();	
				mPVD = NULL;
				PX_RELEASE(transport);
			}
			PX_RELEASE(mFoundation);
		}

		void PhysicsWorld::SetGravity(const Vector3& gravity)
		{
			mScene->setGravity(physx::PxVec3(gravity.x, gravity.y, gravity.z));
		}

		void PhysicsWorld::AddBody(iCollisionBody* body)
		{
			
		}

		void PhysicsWorld::RemoveBody(iCollisionBody* body)
		{
			RigidBody* userRigidBody = static_cast<RigidBody*>(body);
			if (userRigidBody)
			{
				if (userRigidBody->physxRigidbody)
				{
					mScene->removeActor(*userRigidBody->physxRigidbody);
					std::vector< physx::PxRigidActor*>::const_iterator rigidIt;
					for (rigidIt = m_RigidBodies.begin();
						rigidIt != m_RigidBodies.end();
						rigidIt++)
					{
						if (*rigidIt == userRigidBody->physxRigidbody)
							break;
					}
					if(rigidIt != m_RigidBodies.end())
						m_RigidBodies.erase(rigidIt);
				}
			}
		}
		iCharacterController* PhysicsWorld::CreateCharacterController(CharacterControllerDesc desc)
		{
			/*physx::PxShape* actorShape = mPhysics->createShape(
				physx::PxBoxGeometry(
					30.f,
					30.f,
					30.f
				), *mMaterial);*/
			physx::PxShape* actorShape = mPhysics->createShape(
				physx::PxCapsuleGeometry(
					physx::PxReal(desc.radius), physx::PxReal(desc.height)), *mPhysics->createMaterial(0.5, 0.8, 0));
			physx::PxTransform transform(
				physx::PxVec3(desc.position.x, desc.position.y + 2.f, desc.position.z),
				physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1))
			);
			CharacterController* characterController = new CharacterController();
			physx::PxCapsuleControllerDesc controllerDesc;
			controllerDesc.height = desc.height;
			controllerDesc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
			controllerDesc.contactOffset = 0.01f;
			controllerDesc.stepOffset = 0.1f;
			controllerDesc.radius = desc.radius;
			controllerDesc.position = physx::PxExtendedVec3(desc.position.x, desc.position.y, desc.position.z);
			controllerDesc.upDirection = physx::PxVec3(0.0f, 1.0f, 0.0f);
			controllerDesc.material = mPhysics->createMaterial(1.0f, 1.0f, 0.05);
			physx::PxController* controller = mControllerManager->createController(controllerDesc);
			characterController->SetController(controller);
			return characterController;
		}
		void PhysicsWorld::TimeStep(float dt)
		{
			EnterCriticalSection(&UPDATE_LOCK);
			mScene->simulate(dt);
			mScene->fetchResults(true);
			LeaveCriticalSection(&UPDATE_LOCK);
			//const physx::PxRenderBuffer& rb = mScene->getRenderBuffer();
			//for (physx::PxU32 i = 0; i < rb.getNbLines(); i++)
			//{
			//	const physx::PxDebugLine& line = rb.getLines()[i];
			//	
			//	// render the line
			//}
			for (std::vector< physx::PxRigidActor*>::const_iterator rigidIt = m_RigidBodies.begin();
				rigidIt != m_RigidBodies.end();
				rigidIt++)
			{
				RigidBody* userRigidBody = static_cast<RigidBody*>((*rigidIt)->userData);
				physx::PxTransform transform;
				if (!userRigidBody)
					continue;
				if(userRigidBody->IsStatic())
				{
					physx::PxRigidStatic* rigidbody = (physx::PxRigidStatic*)(*rigidIt);
					transform = rigidbody->getGlobalPose();
				}
				else {
					physx::PxRigidBody* rigidbody = (physx::PxRigidBody*)(*rigidIt);
					transform = rigidbody->getGlobalPose();
				}
				userRigidBody->SetRotation(
					{
						transform.q.x,
						transform.q.y,
						transform.q.z,
						transform.q.w
					}
				);
				userRigidBody->SetPosition(
					glm::vec3(
					transform.p.x,
					transform.p.y,
					transform.p.z
				));
			}
		}

		void PhysicsWorld::AddConstraint(iConstraint* constraint)
		{
			if (constraint->GetConstraintType() == ConstraintType::Distance)
			{
				DistanceConstraint* distanceConstraint = DistanceConstraint::Cast(constraint);

				RigidBody* rigidBodyA = (RigidBody*)distanceConstraint->GetRididBodyA();
				RigidBody* rigidBodyB = (RigidBody*)distanceConstraint->GetRididBodyB();
				if (rigidBodyB->joints != nullptr)
				{
					return;
				}
				glm::vec3 translationA = distanceConstraint->GetPositionA();
				glm::vec3 translationB = distanceConstraint->GetPositionB();

				EnterCriticalSection(&JOINTS_LOCK);
				PxInitExtensions(*mPhysics, mPVD);

				physx::PxDistanceJoint* joint = physx::PxDistanceJointCreate(
					*mPhysics,
					rigidBodyA->physxRigidbody,
					physx::PxTransform(physx::PxVec3(translationA.x, translationA.y, translationA.z)),
					rigidBodyB->physxRigidbody,
					physx::PxTransform(physx::PxVec3(translationB.x, translationB.y, translationB.z))
				);
				if (!joint)
					int breakme = 0;
				joint->setDistanceJointFlag(physx::PxDistanceJointFlag::Enum::eMAX_DISTANCE_ENABLED, true);
				joint->setDistanceJointFlag(physx::PxDistanceJointFlag::Enum::eMIN_DISTANCE_ENABLED, true);
				joint->setDistanceJointFlag(physx::PxDistanceJointFlag::Enum::eSPRING_ENABLED, true);
				joint->setMaxDistance(physx::PxReal(2.0f));
				joint->setMinDistance(physx::PxReal(1.1f));
				joint->setStiffness(physx::PxReal(10000.f));
				joint->setConstraintFlag(physx::PxConstraintFlag::eALWAYS_UPDATE, true);
				joint->setContactDistance(0);
				rigidBodyB->joints = joint;
				rigidBodyB->hasJoint = true;
				LeaveCriticalSection(&JOINTS_LOCK);
			}
		}

		void PhysicsWorld::CreateBlastFrameWork()
		{
			blastFramework = NvBlastTkFrameworkCreate();
			if (!blastFramework)
			{
				int breakMeHere = 0;
			}
			CreateBlastAsset();
		}

		void PhysicsWorld::CreateBlastAsset()
		{
			desc = new Nv::Blast::TkAssetDesc();
			desc->chunkCount = 2;

			std::vector<NvBlastChunkDesc> chunkDescs;
			chunkDescs.resize(desc->chunkCount);
			for (int i = 0; i < desc->chunkCount; i++)
			{
				chunkDescs[i].parentChunkDescIndex = i == 0 ? UINT32_MAX : i -1;    // invalid index denotes a chunk hierarchy root
				chunkDescs[i].centroid[0] = i;   // centroid position in asset-local space
				chunkDescs[i].centroid[1] = i * 2;
				chunkDescs[i].centroid[2] = i * 3;
				chunkDescs[i].volume = 1.0f;    // Unit volume
				chunkDescs[i].flags = i == 0? NvBlastChunkDesc::NoFlags : NvBlastChunkDesc::SupportFlag;
				chunkDescs[i].userData = i;
			}
			desc->chunkDescs = &chunkDescs[0];
			desc->bondCount = 2;
			uint8_t* bondFlags = new uint8_t[8]; // Clear all flags
			for (uint32_t i = 0; i < desc->bondCount; ++i)
			{
				bondFlags[i] = Nv::Blast::TkAssetDesc::NoFlags;
			}
			desc->bondFlags = &bondFlags[0];
			std::vector<NvBlastBondDesc> bondDescs;
			bondDescs.resize(desc->bondCount);  // bondCount > 0

			for (int i = 0; i < desc->bondCount; i++)
			{
				bondDescs[i].chunkIndices[0] = i;   // chunkIndices refer to chunk descriptor indices for support chunks
				bondDescs[i].chunkIndices[1] = i + 1;
				bondDescs[i].bond.normal[0] = 1.0f; // normal in the +x direction
				bondDescs[i].bond.normal[1] = 0.0f;
				bondDescs[i].bond.normal[2] = 0.0f;
				bondDescs[i].bond.area = 1.0f;  // unit area
				bondDescs[i].bond.centroid[0] = i + 1.f;   // centroid position in asset-local space
				bondDescs[i].bond.centroid[1] = i + 2.f;
				bondDescs[i].bond.centroid[2] = i + 3.f;
				bondDescs[i].bond.userData = i == desc->bondCount - 1? ~0: i;  // this can be used to tell the user more information about this
				// bond for example to create a joint when this bond breaks
			}
			desc->bondDescs = &bondDescs[0];
			blastAsset = blastFramework->createAsset(*desc);	
			actorDesc = new Nv::Blast::TkActorDesc();
			actorDesc->asset = blastAsset;
			blastActor = blastFramework->createActor(*actorDesc);

			Nv::Blast::TkGroupDesc groupDesc;
			groupDesc.workerCount = 1;  // this example processes groups on the calling thread only

			group = blastFramework->createGroup(groupDesc);
			group->addActor(*blastActor);
		}

		void PhysicsWorld::DamageAsset()
		{
			NvBlastDamageProgram program = { NvBlastExtShearGraphShader, NvBlastExtShearSubgraphShader };

			// The example struct "RadialDamageDesc" is modeled after NvBlastExtRadialDamageDesc in the NvBlastExtShaders extension
			NvBlastExtRadialDamageDesc  damageDescs[2];

			//damageDescs[0].compressive = 10.0f;
			damageDescs[0].position[0] = 1.0f;
			damageDescs[0].position[1] = 2.0f;
			damageDescs[0].position[2] = 3.0f;
			damageDescs[0].minRadius = 0.0f;
			damageDescs[0].maxRadius = 1.0f;

			//damageDescs[1].compressive = 100.0f;
			damageDescs[1].position[0] = 3.0f;
			damageDescs[1].position[1] = 4.0f;
			damageDescs[1].position[2] = 5.0f;
			damageDescs[1].minRadius = 0.0f;
			damageDescs[1].maxRadius = 5.0f;

			// The example material "Material" is modeled after NvBlastExtMaterial in the NvBlastExtShaders extension
			NvBlastExtMaterial  material;

			material.health = 10.0f;
			material.minDamageThreshold = 0.1f;
			material.maxDamageThreshold = 0.8f;

			// Set the damage params struct
			blastParams = new NvBlastProgramParams( damageDescs, 2, &material );

			
			// Apply damage
			blastActor->damage(program, &blastParams);    // params must be kept around until TkGroup::endProcess is called!

			group->process();

			Nv::Blast::TkFamily& family = blastActor->getFamily();
			NvBlastFractureBuffers buffers;
			buffers.bondFractureCount = 2;
			buffers.chunkFractureCount = 2;
			NvBlastLog log;
			NvBlastTimers timers;
			blastActor->generateFracture(&buffers, program, &blastParams);
			blastActor->applyFracture(nullptr, &buffers);
			std::cout << "Actors count = " << group->getActorCount() << std::endl;
			std::cout << "F Actors count = " << family.getActorCount() << std::endl;
			std::cout << "Bond fracture count = " << buffers.bondFractureCount << std::endl;
			std::cout << "Chunk fracture count = " << buffers.chunkFractureCount << std::endl;
		}

		RigidBody* PhysicsWorld::AddActor(const RigidBodyDesc& desc, iShape* shape)
		{
			RigidBody* body = new RigidBody(desc, shape);
			physx::PxShape* actorShape = nullptr;
			mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
			if (shape->GetShapeType() == ShapeType::Sphere)
			{
				SphereShape* sphereShape = dynamic_cast<SphereShape*>(shape);
				actorShape = mPhysics->createShape(
					physx::PxSphereGeometry(
						sphereShape->GetRadius()), *mMaterial);
			}
			if (shape->GetShapeType() == ShapeType::Box)
			{
				BoxShape* boxShape = dynamic_cast<BoxShape*>(shape);
				actorShape = mPhysics->createShape(
					physx::PxBoxGeometry(
						boxShape->GetHalfExtents().x,
						boxShape->GetHalfExtents().y,
						boxShape->GetHalfExtents().z
					), *mMaterial);
			}
			if (shape->GetShapeType() == ShapeType::Cylinder)
			{
				CylinderShape* cylinderShape = dynamic_cast<CylinderShape*>(shape);
				actorShape = mPhysics->createShape(
					physx::PxCapsuleGeometry(
						cylinderShape->GetRadius(),cylinderShape->GetHeight()), *mMaterial);
			}
			if (shape->GetShapeType() == ShapeType::Plane)
			{
				PlaneShape* planeShape = dynamic_cast<PlaneShape*>(shape);
				physx::PxRigidActor* groundPlane = physx::PxCreatePlane(
					*mPhysics, physx::PxPlane(
						planeShape->GetNormal().x, 
						planeShape->GetNormal().y, 
						planeShape->GetNormal().z,
						0.f
						), *mMaterial);
				if (groundPlane)
				{
					mScene->addActor(*groundPlane);
					groundPlane->userData = body;
					body->physxRigidbody = groundPlane;
					m_RigidBodies.push_back(groundPlane);
				}
				return body;
			}
			physx::PxRigidBody* rigidBody = nullptr;
			glm::quat rot = glm::quat(glm::vec3(0.f));
			if (shape->GetShapeType() == ShapeType::Cylinder)
			{
				rot = glm::quat(glm::vec3(0.f, 0.f, -1.57f));
			}
			physx::PxTransform transform(
				physx::PxVec3(desc.position.x, desc.position.y, desc.position.z), { rot.x, rot.y, rot.z, rot.w});
			if (!actorShape)
				return nullptr;

			if (desc.isStatic)
			{
				physx::PxRigidStatic* staticBody = mPhysics->createRigidStatic(transform);
				if (staticBody)
				{
					staticBody->userData = body;
					staticBody->attachShape(*actorShape);
					mScene->addActor(*staticBody);
					body->physxRigidbody = staticBody;
					m_RigidBodies.push_back(staticBody);
					actorShape->release();
				}
			}
			else
			{
				physx::PxRigidDynamic* pseudoBody = mPhysics->createRigidDynamic(transform);
				if (pseudoBody)
				{
					physx::PxRigidBodyExt::updateMassAndInertia(*pseudoBody, desc.mass / desc.volume);
					pseudoBody->setLinearVelocity({ desc.linearVelocity.x, desc.linearVelocity.y, desc.linearVelocity.z });
					pseudoBody->userData = body;
					pseudoBody->attachShape(*actorShape);
					mScene->addActor(*pseudoBody);
					body->physxRigidbody = pseudoBody;
					m_RigidBodies.push_back(pseudoBody);
					actorShape->release();
				}
			}
			return body;
		}
	}
}