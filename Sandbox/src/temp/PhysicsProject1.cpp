#include "PhysicsProject1.h"
#include <PhysicsFactory.h>
#include <RigidBodyDesc.h>
#include <PlaneShape.h>
#include <SphereShape.h>

PhysicsScene::PhysicsScene()
	:m_EditorCamera(45.f, 1280, 720, 0.1f, 1000.f)
{
	m_ShaderLibrary.Load("assets/shaders/simpleshader1.glsl"); 
	std::dynamic_pointer_cast<vel::OpenGLShader>(m_ShaderLibrary.Get("simpleshader1"))->Bind();
	std::dynamic_pointer_cast<vel::OpenGLShader>(m_ShaderLibrary.Get("simpleshader1"))->UploadUniformInt("u_Texture", 0);
	m_PhysicsFactory = new physics::vel::PhysicsFactory();
	m_PhysicsWorld = m_PhysicsFactory->CreateWorld();
	float yValue = 15.f;
	LoadCoordinates();
	CreateWalls();
	CreateGround();
	CreateBall(glm::vec3(0.f, yValue, 0.f), 5, 1.5f, "football");
	CreateBall(glm::vec3(0.f, yValue, -15.f), 10, 2.f, "cyan");
	CreateBall(glm::vec3(-15.f, yValue, 0.f), 15, 3.5f, "cyan");
	CreateBall(glm::vec3(15.f, yValue, -5.f), 20, 2.f, "cyan");
	CreateBall(glm::vec3(15.f, yValue, -15.f), 3, 1.f, "yellow");
	
	vel::RenderCommand::Init();
	vel::RenderCommand::SetDepthMask(true);
	float gravity = 0.0981f;
	m_PhysicsWorld->SetGravity(glm::vec3(0.f, -gravity , 0.f));
	vel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
}

void PhysicsScene::OnUpdate(vel::Timestep ts)
{
	m_EditorCamera.OnUpdate(ts);
	if(m_PhysicsWorld)
		m_PhysicsWorld->TimeStep(ts);

	vel::RenderCommand::Clear();
	vel::RenderCommand::SetCullFace();
	float ratio = (float) vel::Application::Get().GetWindow().GetWidth() / vel::Application::Get().GetWindow().GetHeight();
	glm::mat4 matView = glm::lookAt(CameraPosition, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	
	
	glm::mat4 matProjection = glm::perspective(
		FOV,
		ratio,
		0.1f,
		10000.0f);

	vel::Renderer::BeginScene(matProjection * matView);

	for (GameObject* gameObject : m_GameObjects)
	{
		m_ShaderLibrary.Get("simpleshader1")->Bind();
		if (controlBall)
		{
			glm::vec3 rigidBodyPosition;
			controlBall->GetPosition(rigidBodyPosition);
			if (vel::Input::IsKeyPressed(KeyCode::Space))
			{
				controlBall->ApplyForce(glm::vec3(0.f, 1.f,0.f));
			}
			if (vel::Input::IsKeyPressed(KeyCode::Up))
			{
				if (FOV < 0.9f)
				{
					FOV += 0.001f;
				}
			}
			if (vel::Input::IsKeyPressed(KeyCode::Down))
			{
				if (FOV > 0.2f)
				{
					FOV -= 0.001f;
				}
			}

			if (vel::Input::IsKeyPressed(KeyCode::Left))
			{
				glm::vec3 forward = glm::vec3(0.f) - CameraPosition;
				glm::mat4 rotationMat(1);
				rotationMat = glm::rotate(rotationMat, -90.0f, glm::vec3(0.0, 1.0, 0.0));
				glm::vec3 rotated = glm::vec3(rotationMat * glm::vec4(forward, 1.0));


				glm::vec3 updateValue = rotated -  CameraPosition;

				CameraPosition = updateValue;
			}
			if (vel::Input::IsKeyPressed(KeyCode::Right))
			{
				glm::vec3 forward = CameraPosition - glm::vec3(0.f);
				glm::mat4 rotationMat(1);
				rotationMat = glm::rotate(rotationMat, 90.0f, glm::vec3(0.0, 1.0, 0.0));
				forward = glm::vec3(rotationMat * glm::vec4(forward, 1.0));

				glm::vec3 updateValue = forward -  CameraPosition;

				CameraPosition += updateValue;
			}

			if (vel::Input::IsKeyPressed(KeyCode::W))
			{
				glm::vec3 forward = glm::normalize(rigidBodyPosition - CameraPosition);
				float x = forward.x * 2.f;
				float z = forward.z * 2.f;
				controlBall->ApplyForce(
					glm::vec3(
						x,
						0.f,
						z
					)
				);
			}
			if (vel::Input::IsKeyPressed(KeyCode::S))
			{
				glm::vec3 forward = glm::normalize(rigidBodyPosition - CameraPosition);
				float x = forward.x * 2.f;
				float z = forward.z * 2.f;
				controlBall->ApplyForce(
					glm::vec3(
						-x,
						0.f,
						-z
					)
				);
			}
			if (vel::Input::IsKeyPressed(KeyCode::A))
			{
				glm::vec3 forward = glm::normalize(rigidBodyPosition - CameraPosition);

				glm::mat4 rotationMat(1);
				rotationMat = glm::rotate(rotationMat, 90.0f, glm::vec3(0.0, 1.0, 0.0));
				glm::vec3 right = glm::vec3(rotationMat * glm::vec4(forward, 1.0));

				float x = right.x * 2.f;
				float z = right.z * 2.f;
				controlBall->ApplyForce(
					glm::vec3(
						-x,
						0.f,
						-z
					)
				);
			}
			if (vel::Input::IsKeyPressed(KeyCode::D))
			{
				glm::vec3 forward = glm::normalize(rigidBodyPosition - CameraPosition);

				glm::mat4 rotationMat(1);
				rotationMat = glm::rotate(rotationMat, 90.0f, glm::vec3(0.0, 1.0, 0.0));
				glm::vec3 right = glm::vec3(rotationMat * glm::vec4(forward, 1.0));

				float x = right.x * 2.f;
				float z = right.z * 2.f;
				controlBall->ApplyForce(
					glm::vec3(
						x,
						0.f,
						z
					)
				);
			}
			/*
			if (vel::Input::IsKeyPressed(KeyCode::Left))
			{
				controlBall->ApplyForce(glm::vec3(-5.f, 0.f, 0.f) * m_EditorCamera.GetForwardDirection());
			}
			if (vel::Input::IsKeyPressed(KeyCode::Right))
			{
				controlBall->ApplyForce(glm::vec3(5.f, 0.f, 0.f));
			}
			if (vel::Input::IsKeyPressed(KeyCode::Up))
			{
				controlBall->ApplyForce(glm::vec3(0.f, 0.f, -5.f));
			}
			if (vel::Input::IsKeyPressed(KeyCode::Down))
			{
				controlBall->ApplyForce(glm::vec3(0.f, 0.f, 5.f));
			}*/
		}
		glm::mat4x4 matModel = glm::mat4x4(1.0f);
		if(gameObject->rigidBody)
			gameObject->rigidBody->GetPosition(gameObject->transform.position); 

		glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f),
			gameObject->transform.position);
		glm::mat4 matQRotation = glm::mat4(gameObject->transform.rotation);
		glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
			gameObject->transform.scale);

		matModel = matModel * matTranslation;
		matModel = matModel * matQRotation;
		matModel = matModel * matScale;

		if (gameObject->name == "wall")
		{
			vel::RenderCommand::SetDepthMask(true);
			//continue;
			std::dynamic_pointer_cast<vel::OpenGLShader>
				(m_ShaderLibrary.Get("simpleshader1"))->UploadUniformFloat("isAWall", 1.f);
		}
		else
		{
			//vel::RenderCommand::SetDepthMask(false);
			std::dynamic_pointer_cast<vel::OpenGLShader>
				(m_ShaderLibrary.Get("simpleshader1"))->UploadUniformFloat("isAWall", 0.f);
		}
		gameObject->m_Texture->Bind(0);

		vel::RenderCommand::EnableDepth();
		vel::RenderCommand::SetCullFace();
		vel::Renderer::Submit(m_ShaderLibrary.Get("simpleshader1"), gameObject->m_VertexArray, matModel);
	}
	vel::Renderer::EndScene();

}

void PhysicsScene::OnImGuiRender()
{
}

void PhysicsScene::OnEvent(vel::Event& event)
{
	m_EditorCamera.OnEvent(event);
	if (event.GetEventType() == vel::EventType::WindowResize)
	{
		auto& we = (vel::WindowResizeEvent&)event;
		m_EditorCamera.SetViewportSize(we.GetWidth(), we.GetHeight());
	}
}

void PhysicsScene::CreateBall(glm::vec3 position, float size,float radius, std::string color)
{
	GameObject* gameObejct = new GameObject();
	gameObejct->transform.position = position;
	gameObejct->transform.rotation = glm::quat(glm::vec3(0.f));
	gameObejct->transform.scale = glm::vec3(radius);
	LoadPlyFiles("assets/models/ISO_Shphere_flat_4div_xyz_n_rgba_uv.ply", gameObejct);

	gameObejct->m_Texture = vel::Texture2D::Create("assets/textures/" + color +".png");

	physics::iShape* ballShape = new physics::SphereShape(radius);
	physics::RigidBodyDesc desc;
	desc.isStatic = false;
	desc.mass = size;
	desc.position = position;
	desc.angularVelocity = glm::vec3(0.f);
	desc.rotation = gameObejct->transform.rotation;
	desc.linearVelocity = glm::vec3(0.f);
	gameObejct->rigidBody = m_PhysicsFactory->CreateRigidBody(desc, ballShape);

	m_GameObjects.push_back(gameObejct);
	//gameObejct->rigidBody->SetRenderPosition(&gameObejct->transform.position);
	m_PhysicsWorld->AddBody(gameObejct->rigidBody);
	controlBall = gameObejct->rigidBody;
}

// Christer Ericson - Real-time collision detection
// Page number 55
physics::PlaneShape* ComputePlane(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC)
{
	glm::vec3 normal = glm::normalize(glm::cross(pointB - pointA, pointC - pointA));
	float dot = glm::dot(normal, pointA);
	return new physics::PlaneShape(dot, normal);
}

void PhysicsScene::CreateGround()
{
	GameObject* gameObejct = new GameObject();
	gameObejct->name = "ground";
	gameObejct->transform.position = glm::vec3(0.f, 0.f, 0.f);
	gameObejct->transform.rotation = glm::angleAxis(glm::radians(-90.f), glm::vec3(1, 0, 0));
	gameObejct->transform.scale = glm::vec3(40.f, 40.f, 1.f);
	LoadPlyFiles("assets/models/quad.ply", gameObejct);
	gameObejct->m_Texture = vel::Texture2D::Create("assets/textures/ground.png");
	physics::RigidBodyDesc desc;
	//physics::iShape* planeShape = new physics::PlaneShape(1.0f, glm::vec3(0.f, 1.f, 0.f));
	physics::iShape* planeShape = ComputePlane(
									glm::vec3(-40.f, 0.f, -40.f),
									glm::vec3(40.f, 0.f, -40.f),
									glm::vec3(40.f, 0.f, 40.f)
								);
	desc.isStatic = true;
	desc.mass = 0;
	desc.position = glm::vec3(0.f, 0.f, 0.f);
	desc.linearVelocity = glm::vec3(0.f);
	gameObejct->rigidBody = m_PhysicsFactory->CreateRigidBody(desc, planeShape);
	m_GameObjects.push_back(gameObejct);
	//gameObejct->rigidBody->SetRenderPosition(&gameObejct->transform.position);
	m_PhysicsWorld->AddBody(gameObejct->rigidBody);
}

void PhysicsScene::CreateWalls()
{
	// Sealing
	{
		GameObject* gameObejct = new GameObject();
		gameObejct->name = "wall";
		gameObejct->transform.position = glm::vec3(0.f, 20.f, 0.f);
		gameObejct->transform.rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1, 0, 0));
		gameObejct->transform.scale = glm::vec3(40.f, 40.f, 1.f);
		LoadPlyFiles("assets/models/quad.ply", gameObejct);
		gameObejct->m_Texture = vel::Texture2D::Create("assets/textures/ground.png");
		physics::RigidBodyDesc desc;
		physics::iShape* planeShape = ComputePlane(
			glm::vec3(-40.f, 20.f, -40.f),
			glm::vec3(40.f, 20.f, -40.f),
			glm::vec3(40.f, 20.f, 40.f)
		);
		desc.isStatic = true;
		desc.mass = 0;
		desc.position = glm::vec3(0.f, 20.f, 0.f);
		desc.linearVelocity = glm::vec3(0.f);
		gameObejct->rigidBody = m_PhysicsFactory->CreateRigidBody(desc, planeShape);
		m_GameObjects.push_back(gameObejct);
		//gameObejct->rigidBody->SetRenderPosition(&gameObejct->transform.position);
		m_PhysicsWorld->AddBody(gameObejct->rigidBody);
	}
	// +z
	{
		GameObject* gameObejct = new GameObject();
		gameObejct->name = "wall";
		gameObejct->transform.position = glm::vec3(0.f, 0.f, 40.f);
		gameObejct->transform.rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(1, 0, 0));
		gameObejct->transform.scale = glm::vec3(40.f, 40.f, 1.f);
		LoadPlyFiles("assets/models/quad.ply", gameObejct);
		gameObejct->m_Texture = vel::Texture2D::Create("assets/textures/green.png");
		physics::RigidBodyDesc desc;
		physics::iShape* planeShape = ComputePlane(
			glm::vec3(-40.f, 20.f, 40.f),
			glm::vec3(40.f, 20.f, 40.f),
			glm::vec3(40.f, -20.f, 40.f)
		);
		desc.isStatic = true;
		desc.mass = 0;
		desc.position = glm::vec3(0.f, 0.f, 40.f);
		desc.linearVelocity = glm::vec3(0.f);
		gameObejct->rigidBody = m_PhysicsFactory->CreateRigidBody(desc, planeShape);
		m_GameObjects.push_back(gameObejct);
		//gameObejct->rigidBody->SetRenderPosition(&gameObejct->transform.position);
		m_PhysicsWorld->AddBody(gameObejct->rigidBody);
	}
	// -z
	{
		GameObject* gameObejct = new GameObject();
		gameObejct->name = "wall";
		gameObejct->transform.position = glm::vec3(0.f, 0.f, -40.f);
		gameObejct->transform.rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(1, 0, 0));
		gameObejct->transform.scale = glm::vec3(40.f, 40.f, 1.f);
		LoadPlyFiles("assets/models/quad.ply", gameObejct);
		gameObejct->m_Texture = vel::Texture2D::Create("assets/textures/green.png");
		physics::RigidBodyDesc desc;
		physics::iShape* planeShape = ComputePlane(
			glm::vec3(-40.f, 20.f, -40.f),
			glm::vec3(40.f, 20.f, -40.f),
			glm::vec3(40.f, -20.f, -40.f)
		);
		desc.isStatic = true;
		desc.mass = 0;
		desc.position = glm::vec3(0.f, 0.f, -40.f);
		desc.linearVelocity = glm::vec3(0.f);
		gameObejct->rigidBody = m_PhysicsFactory->CreateRigidBody(desc, planeShape);
		m_GameObjects.push_back(gameObejct);
		//gameObejct->rigidBody->SetRenderPosition(&gameObejct->transform.position);
		m_PhysicsWorld->AddBody(gameObejct->rigidBody);
	}
	// +x
	{
		GameObject* gameObejct = new GameObject();
		gameObejct->name = "wall";
		gameObejct->transform.position = glm::vec3(40.f, 0.f, 0.f);
		gameObejct->transform.rotation = glm::angleAxis(glm::radians(-90.f), glm::vec3(0, 1, 0));
		gameObejct->transform.scale = glm::vec3(40.f, 40.f, 1.f);
		LoadPlyFiles("assets/models/quad.ply", gameObejct);
		gameObejct->m_Texture = vel::Texture2D::Create("assets/textures/red.png");
		physics::RigidBodyDesc desc;
		physics::iShape* planeShape = ComputePlane(
			glm::vec3(40.f, 20.f, -40.f),
			glm::vec3(40.f, 20.f, 40.f),
			glm::vec3(40.f, -20.f, -40.f)
		);
		desc.isStatic = true;
		desc.mass = 0;
		desc.position = glm::vec3(40.f, 0.f, 0.f);
		desc.linearVelocity = glm::vec3(0.f);
		gameObejct->rigidBody = m_PhysicsFactory->CreateRigidBody(desc, planeShape);
		m_GameObjects.push_back(gameObejct);
		//gameObejct->rigidBody->SetRenderPosition(&gameObejct->transform.position);
		m_PhysicsWorld->AddBody(gameObejct->rigidBody);
	}
	// -x
	{
		GameObject* gameObejct = new GameObject();
		gameObejct->name = "wall";
		gameObejct->transform.position = glm::vec3(-40.f, 0.f, 0.f);
		gameObejct->transform.rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0, 1, 0));
		gameObejct->transform.scale = glm::vec3(40.f, 40.f, 1.f);
		LoadPlyFiles("assets/models/quad.ply", gameObejct);
		gameObejct->m_Texture = vel::Texture2D::Create("assets/textures/red.png");
		physics::RigidBodyDesc desc;
		physics::iShape* planeShape = ComputePlane(
			glm::vec3(-40.f, 20.f, -40.f),
			glm::vec3(-40.f, 20.f, 40.f),
			glm::vec3(-40.f, -20.f, -40.f)
		);
		desc.isStatic = true;
		desc.mass = 0;
		desc.position = glm::vec3(-40.f, 0.f, 0.f);
		desc.linearVelocity = glm::vec3(0.f);
		gameObejct->rigidBody = m_PhysicsFactory->CreateRigidBody(desc, planeShape);
		m_GameObjects.push_back(gameObejct);
		//gameObejct->rigidBody->SetRenderPosition(&gameObejct->transform.position);
		m_PhysicsWorld->AddBody(gameObejct->rigidBody);
	}
}

void PhysicsScene::LoadCoordinates()
{
	GameObject* xgameObejct = new GameObject();
	xgameObejct->transform.position = glm::vec3(0.f, 30.f, 0.f);
	xgameObejct->transform.rotation = glm::quat(glm::vec3(0.f));
	xgameObejct->transform.scale = glm::vec3(10.f);
	LoadPlyFiles("assets/models/engine/x.ply", xgameObejct);
	m_GameObjects.push_back(xgameObejct);

	xgameObejct->m_Texture = vel::Texture2D::Create("assets/textures/red.png");

	GameObject* ygameObejct = new GameObject();
	ygameObejct->transform.position = glm::vec3(0.f, 30.f, 0.f);
	ygameObejct->transform.rotation = glm::quat(glm::vec3(0.f));
	ygameObejct->transform.scale = glm::vec3(10.f);
	LoadPlyFiles("assets/models/engine/y.ply", ygameObejct);
	m_GameObjects.push_back(ygameObejct);

	ygameObejct->m_Texture = vel::Texture2D::Create("assets/textures/blue.png");

	GameObject* zgameObejct = new GameObject();
	zgameObejct->transform.position = glm::vec3(0.f, 30.f, 0.f);
	zgameObejct->transform.rotation = glm::quat(glm::vec3(0.f));
	zgameObejct->transform.scale = glm::vec3(10.f);
	LoadPlyFiles("assets/models/engine/z.ply", zgameObejct);
	m_GameObjects.push_back(zgameObejct);

	zgameObejct->m_Texture = vel::Texture2D::Create("assets/textures/green.png");
}

bool PhysicsScene::LoadPlyFiles(std::string fileName, GameObject* gameObject)
{
	struct sVertex_XYZ_N_RGBA_UV
	{
		float x, y, z;
		float nx, ny, nz;

		float red, green, blue, alpha;
		float texture_u, texture_v;
	};

	struct sTrianglePLY
	{
		unsigned int vertexIndices[3];
	};

	sVertex_XYZ_N_RGBA_UV* pTheModelArray = NULL;
	sTrianglePLY* pTheModelTriangleArray = NULL;

	std::ifstream theFile(fileName);
	if (!theFile.is_open())
	{
		std::cout << "Couldn\'t open the model file";
		return false;
	}

	char buffer[10000];
	theFile.getline(buffer, 10000);

	std::string theNextToken;

	while (theFile >> theNextToken)
		if (theNextToken == "vertex")
			break;

	unsigned int numberOfVertices;
	theFile >> numberOfVertices;

	bool hasColours = false;
	while (theFile >> theNextToken)
	{
		if (theNextToken == "red")
			hasColours = true;
		if (theNextToken == "face")
			break;
	}

	unsigned int numberOfTriangles;
	theFile >> numberOfTriangles;

	while (theFile >> theNextToken)
		if (theNextToken == "end_header")
			break;

	pTheModelArray = new sVertex_XYZ_N_RGBA_UV[numberOfVertices];

	std::cout << "Loading";
	for (unsigned int count = 0; count != numberOfVertices; count++)
	{
		theFile >> pTheModelArray[count].x;
		theFile >> pTheModelArray[count].y;
		theFile >> pTheModelArray[count].z;

		theFile >> pTheModelArray[count].nx;
		theFile >> pTheModelArray[count].ny;
		theFile >> pTheModelArray[count].nz;

		if (hasColours)
		{
			theFile >> pTheModelArray[count].red;
			theFile >> pTheModelArray[count].green;
			theFile >> pTheModelArray[count].blue;
			theFile >> pTheModelArray[count].alpha;
		}


		theFile >> pTheModelArray[count].texture_u;
		theFile >> pTheModelArray[count].texture_v;

		if (count % 10000 == 0)
			std::cout << ".";

	}

	std::cout << " done" << std::endl;

	pTheModelTriangleArray = new sTrianglePLY[numberOfTriangles];

	for (unsigned int count = 0; count != numberOfTriangles; count++)
	{
		// to discard first value
		unsigned int discard = 0;
		theFile >> discard;

		theFile >> pTheModelTriangleArray[count].vertexIndices[0];
		theFile >> pTheModelTriangleArray[count].vertexIndices[1];
		theFile >> pTheModelTriangleArray[count].vertexIndices[2];
	}

	theFile.close();

	vel::sVertex_RGBA_XYZ_N_UV_T_BiN_Bones* pVertices = new vel::sVertex_RGBA_XYZ_N_UV_T_BiN_Bones[numberOfVertices];
	glm::vec3 minPoints = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 maxPoints = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);
	for (unsigned int index = 0; index != numberOfVertices; index++)
	{
		// To shader								from File
		pVertices[index].x = pTheModelArray[index].x;
		pVertices[index].y = pTheModelArray[index].y;
		pVertices[index].z = pTheModelArray[index].z;

		if (minPoints.x > pTheModelArray[index].x)
			minPoints.x = pTheModelArray[index].x;

		if (minPoints.y > pTheModelArray[index].y)
			minPoints.y = pTheModelArray[index].y;

		if (minPoints.z > pTheModelArray[index].z)
			minPoints.z = pTheModelArray[index].z;

		if (maxPoints.x < pTheModelArray[index].x)
			maxPoints.x = pTheModelArray[index].x;

		if (maxPoints.y < pTheModelArray[index].y)
			maxPoints.y = pTheModelArray[index].y;

		if (maxPoints.z < pTheModelArray[index].z)
			maxPoints.z = pTheModelArray[index].z;

		pVertices[index].r = pTheModelArray[index].red;
		pVertices[index].g = pTheModelArray[index].green;
		pVertices[index].b = pTheModelArray[index].blue;

		pVertices[index].nx = pTheModelArray[index].nx;
		pVertices[index].ny = pTheModelArray[index].ny;
		pVertices[index].nz = pTheModelArray[index].nz;

		// Copy the texture coordinates we loaded
		pVertices[index].u0 = pTheModelArray[index].texture_u;
		pVertices[index].v0 = pTheModelArray[index].texture_v;
	}
	std::cout << fileName << " Min values: " << minPoints.x << ", "
		<< minPoints.y << ", " << minPoints.z << "\nMax values: " << maxPoints.x << ", "
		<< maxPoints.y << ", " << maxPoints.z << std::endl;
	unsigned int numberOfIndices = numberOfTriangles * 3;
	glm::vec3 minValues = minPoints;
	glm::vec3 maxValues = maxPoints;

	unsigned int* pIndices = new unsigned int[numberOfIndices];

	unsigned int vertex_element_index_index = 0;

	for (unsigned int triangleIndex = 0; triangleIndex != numberOfTriangles; triangleIndex++)
	{
		pIndices[vertex_element_index_index + 0] = pTheModelTriangleArray[triangleIndex].vertexIndices[0];
		pIndices[vertex_element_index_index + 1] = pTheModelTriangleArray[triangleIndex].vertexIndices[1];
		pIndices[vertex_element_index_index + 2] = pTheModelTriangleArray[triangleIndex].vertexIndices[2];

		vertex_element_index_index += 3;
	}

	gameObject->m_VertexArray = vel::VertexArray::Create();
	vel::Ref<vel::VertexBuffer> vertexBuffer;
	vertexBuffer = vel::VertexBuffer::Create(pVertices, sizeof(vel::sVertex_RGBA_XYZ_N_UV_T_BiN_Bones) * numberOfVertices);
	vertexBuffer->SetLayout({
			{ vel::ShaderDataType::Float4, "vColour"},
			{ vel::ShaderDataType::Float4, "vPosition"},
			{ vel::ShaderDataType::Float4, "vNormal"},
			{ vel::ShaderDataType::Float4, "vUVx2"},
			{ vel::ShaderDataType::Float4, "vTangent"},
			{ vel::ShaderDataType::Float4, "vBiNormal"},
			{ vel::ShaderDataType::Float4, "vBoneID"},
			{ vel::ShaderDataType::Float4, "vBoneWeight"}
		});
	gameObject->m_VertexArray->AddVertexBuffer(vertexBuffer);

	vel::Ref<vel::IndexBuffer> indexBuffer;

	indexBuffer = vel::IndexBuffer::Create(pIndices, numberOfIndices);
	gameObject->m_VertexArray->SetIndexBuffer(indexBuffer);
	gameObject->m_VertexArray->Unbind();


	delete[] pTheModelArray;
	delete[] pTheModelTriangleArray;
	return true;
}

Transform::Transform()
{
	scale = glm::vec3(1.f);
	rotation = glm::quat(glm::vec3(0.f));
	position = glm::vec3(0.f);
}
