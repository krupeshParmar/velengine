#include "pch.h"
#include "PhysicsWorld.h"
#include <BoxShape.h>
#include <CylinderShape.h>
#include "CharacterController.h"
#include <characterkinematic/PxController.h>
namespace physics
{
	namespace physxim
	{
		PhysicsWorld::PhysicsWorld()
			: iPhysicsWorld()
			, m_CollisionHandler(nullptr)
		{
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
		}

		PhysicsWorld::~PhysicsWorld()
		{
			m_RigidBodies.clear();
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
				physx::PxVec3(desc.position.x, desc.position.y, desc.position.z),
				physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1))
			);
			CharacterController* characterController = new CharacterController();
			physx::PxCapsuleControllerDesc controllerDesc;
			controllerDesc.height = 1.8f;
			controllerDesc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
			controllerDesc.contactOffset = 0.05f;
			controllerDesc.stepOffset = 0.1f;
			controllerDesc.radius = 0.25f;
			controllerDesc.position = physx::PxExtendedVec3(desc.position.x, desc.position.y, desc.position.z);
			controllerDesc.upDirection = physx::PxVec3(0.0f, 1.0f, 0.0f);
			controllerDesc.material = mPhysics->createMaterial(1.0f, 1.0f, 0.05);
			physx::PxController* controller = mControllerManager->createController(controllerDesc);
			characterController->SetController(controller);
			return characterController;
		}
		void PhysicsWorld::TimeStep(float dt)
		{
			mScene->simulate(dt);
			mScene->fetchResults(true);
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
				return body;

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