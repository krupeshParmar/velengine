#include "PhysicsProject1.h"
#include <PhysicsFactory.h>
#include <RigidBodyDesc.h>
#include <PlaneShape.h>
#include <SphereShape.h>
#include <imgui/imgui.h>

static vel::Ref<vel::FrameBuffer> framebuffer;
PhysicsScene::PhysicsScene()
	:m_EditorCamera(45.f, 1280, 720, 0.1f, 1000.f)
{
	VEL_PROFILE_FUNCTION();

	{
		VEL_PROFILE_SCOPE("Physics Constructor");
		userInputs = "\nLeft - Right: To rotate camera around the center";
		userInputs += "\n\nUp - Down: To zoom in and out";
		userInputs += "\n\nWASD: To control the selected ball relative to the camera";
		userInputs += "\n\n1 to 5: To select balls from one to five";

		m_ShaderLibrary.Load("assets/shaders/simpleshader1.glsl");
		m_ShaderLibrary.Load("assets/shaders/FBOTexture.glsl");

		m_ShaderLibrary.Get("simpleshader1")->Bind();
		m_ShaderLibrary.Get("simpleshader1")->SetInt("u_Texture", 0);

		m_ShaderLibrary.Get("FBOTexture")->Bind();
		m_ShaderLibrary.Get("FBOTexture")->SetInt("u_Texture", 0);

		m_PhysicsFactory = new physics::vel::PhysicsFactory();
		m_PhysicsWorld = m_PhysicsFactory->CreateWorld();
		float yValue = 25.f;
		CreateWalls();
		CreateGround();

		ballPrefab = new GameObject();
		ballPrefab->name = "ballprefab";
		LoadPlyFiles("assets/models/ball2.ply", ballPrefab);

		quadPrefab = new GameObject();
		LoadPlyFiles("assets/models/quad.ply", quadPrefab);

		ball1info = "\nBall 1 ( 1.11, 2.5 ): A football";
		ball1 = CreateBall(glm::vec3(0.f, yValue, 0.f), 1.11f, 2.5f, "football");

		ball2info = "\nBall 2 ( 0.8, 5.5 ): A beach ball, big but really light";
		ball2 = CreateBall(glm::vec3(30.f, yValue, -30.f), 0.80f, 5.5f, "beach");

		ball3info = "\nBall 3 ( 8.0, 1.85 ): A bowling ball, small but heavy";
		ball3 = CreateBall(glm::vec3(-20.f, yValue, 20.f), 8.0f, 1.85f, "bowling");

		ball4info = "\nBall 4 ( 25.0, 5.0 ): A boulder, really big and heavy";
		ball4 = CreateBall(glm::vec3(25.f, yValue, -15.f), 25.f, 5.f, "boulder");

		ball5info = "\nBall 5 ( 2.4, 3.5 ): A basketball, big but not so heavy";
		ball5 = CreateBall(glm::vec3(15.f, yValue, -35.f), 2.40, 3.5f, "basketball");

		controlBall = ball1;
		vel::RenderCommand::Init();
		vel::RenderCommand::SetDepthMask(true);
		float gravity = 0.981f;
		m_PhysicsWorld->SetGravity(glm::vec3(0.f, -gravity, 0.f));
		vel::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		{
			vel::FrameBufferSpecification fbSpec;
			fbSpec.Width = vel::Application::Get().GetWindow().GetWidth();
			fbSpec.Height = vel::Application::Get().GetWindow().GetHeight();
			framebuffer = vel::FrameBuffer::Create(fbSpec);
			fullScreenFrameBuffer = vel::FrameBuffer::Create(fbSpec);
			m_SquareVertexArray = vel::VertexArray::Create();
			float sqVertices[6 * 4] = {
				0.0f, 0.0f,	-0.75,	-0.75f,	0.0f,	1.f,
				1.0f, 0.0f, 0.75f,	-0.75f,	0.0f,	1.f,
				1.0f, 1.0f, 0.75f,	0.75f,	0.0f,	1.f,
				0.0f, 1.0f, -0.75f,	0.75f,	0.0f,	1.f
			};
			m_SquareVertexArray = vel::VertexArray::Create();
			vel::Ref<vel::VertexBuffer> squareVB;
			squareVB = (vel::VertexBuffer::Create(sqVertices, sizeof(sqVertices)));

			squareVB->SetLayout({
					{ vel::ShaderDataType::Float2, "a_TextureCoords"},
					{ vel::ShaderDataType::Float4, "a_Position"}
				});
			m_SquareVertexArray->AddVertexBuffer(squareVB);

			uint32_t sq_indices[6] = {
				0, 1, 2, 2, 3, 0
			};

			// index buffer
			vel::Ref<vel::IndexBuffer> squareIB;
			squareIB = (vel::IndexBuffer::Create(sq_indices, sizeof(sq_indices) / sizeof(uint32_t)));
			m_SquareVertexArray->SetIndexBuffer(squareIB);
		}
	}
}
void PhysicsScene::OnImGuiRender()
{
	VEL_PROFILE_FUNCTION();
	float old_size = ImGui::GetFont()->Scale;
	ImGui::GetFont()->Scale *= 1.4;
	ImGui::PushFont(ImGui::GetFont());
	ImGui::Begin("Info");
	ImGui::BeginChild(
		"Info",
		ImVec2(
			vel::Application::Get().GetWindow().GetWidth() / 4,
			vel::Application::Get().GetWindow().GetHeight() / 2
		),
		true
		);
	ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.8f, 1.f), "To select a ball, press number 1 to 5");
	ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.8f, 1.f), "Currently selected = ");

	ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.f), "Balls (Mass, Radius)");
	

	ImGui::Text(CurrentlySelectedBall().c_str());

	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.8f, 1.f), "User Inputs:");

	ImGui::Text(userInputs.c_str());
	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.f), "Restart if you don't see any balls.");

	ImGui::EndChild();
	ImGui::End();
	ImGui::GetFont()->Scale = old_size;
	ImGui::PopFont();
	ImGui::Begin("ViewPort");
	uint32_t textureID = fullScreenFrameBuffer->GetColorAttachmenRendererID();
	ImGui::Image((void*)textureID, ImVec2{ 2160, 1440 });
	ImGui::End();
}

void PhysicsScene::OnUpdate(vel::Timestep ts)
{
	VEL_PROFILE_FUNCTION();
	m_EditorCamera.OnUpdate(ts);
	framebuffer->Bind();

	vel::RenderCommand::EnableDepth();
	{
		VEL_PROFILE_SCOPE("Physics World Update");
		if (m_PhysicsWorld)
			m_PhysicsWorld->TimeStep(ts.GetSeconds());
	}

	{
		VEL_PROFILE_SCOPE("Render Draw");
		vel::RenderCommand::Clear();
		vel::RenderCommand::SetCullFace();
		float ratio = (float)vel::Application::Get().GetWindow().GetWidth() / vel::Application::Get().GetWindow().GetHeight();

		glm::mat4 matView = glm::lookAt(
			CameraPosition,
			glm::vec3(0.f),
			glm::vec3(0.f, 1.f, 0.f)
		);


		glm::mat4 matProjection = glm::perspective(
			FOV,
			ratio,
			10.f,
			200.0f);

		vel::Renderer::BeginScene(matProjection * matView);

		for (GameObject* gameObject : m_GameObjects)
		{
			m_ShaderLibrary.Get("simpleshader1")->Bind();
			DetectChangeInControllBall();
			if (controlBall)
			{
				glm::vec3 rigidBodyPosition;
				controlBall->GetPosition(rigidBodyPosition);

				HandleCamera();

				float force = 0.3f;
				glm::vec3 direction(0.f);
				if (vel::Input::IsKeyPressed(KeyCode::W))
				{
					glm::vec3 forward = glm::normalize(rigidBodyPosition - CameraPosition);
					float x = forward.x * 2.f;
					float z = forward.z * 2.f;
					direction = glm::vec3(
						x,
						0.f,
						z
					);
				}
				if (vel::Input::IsKeyPressed(KeyCode::S))
				{
					glm::vec3 forward = glm::normalize(rigidBodyPosition - CameraPosition);
					float x = forward.x * 2.f;
					float z = forward.z * 2.f;
					direction = glm::vec3(
						-x,
						0.f,
						-z
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

					direction = glm::vec3(x,
						0.f,
						z
					);
				}
				if (vel::Input::IsKeyPressed(KeyCode::D))
				{
					glm::vec3 forward = glm::normalize(rigidBodyPosition - CameraPosition);

					glm::mat4 rotationMat(1);
					rotationMat = glm::rotate(rotationMat, -90.0f, glm::vec3(0.0, 1.0, 0.0));
					glm::vec3 right = glm::vec3(rotationMat * glm::vec4(forward, 1.0));

					float x = right.x * 2.f;
					float z = right.z * 2.f;

					direction = glm::vec3(
						x,
						0.f,
						z
					);
				}

				controlBall->ApplyForce(
					direction
				);

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
			if (gameObject->rigidBody)
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
				std::dynamic_pointer_cast<vel::OpenGLShader>
					(m_ShaderLibrary.Get("simpleshader1"))->UploadUniformFloat("isAWall", 1.f);
			}
			else
			{
				std::dynamic_pointer_cast<vel::OpenGLShader>
					(m_ShaderLibrary.Get("simpleshader1"))->UploadUniformFloat("isAWall", 0.f);
			}
			gameObject->m_Texture->Bind(0);

			vel::RenderCommand::SetCullFace();
			if (gameObject->name == "ball")
				vel::Renderer::Submit(m_ShaderLibrary.Get("simpleshader1"), ballPrefab->m_VertexArray, matModel);
			else if (gameObject->name == "ground" || gameObject->name == "wall")
				vel::Renderer::Submit(m_ShaderLibrary.Get("simpleshader1"), quadPrefab->m_VertexArray, matModel);
			else vel::Renderer::Submit(m_ShaderLibrary.Get("simpleshader1"), gameObject->m_VertexArray, matModel);
		}
		vel::Renderer::EndScene();
		framebuffer->Unbind();
		fullScreenFrameBuffer->Bind();
		vel::RenderCommand::DisableDepth();
		vel::RenderCommand::Clear();
		framebuffer->BindColorTexture();
		vel::Renderer::Submit(
			m_ShaderLibrary.Get("FBOTexture"),
			m_SquareVertexArray,
			glm::scale(glm::mat4(1.0f), glm::vec3(100.f))
			);
		fullScreenFrameBuffer->Unbind();
	}

}

void PhysicsScene::OnEvent(vel::Event& event)
{
	m_EditorCamera.OnEvent(event);
	if (event.GetEventType() == vel::EventType::WindowResize)
	{
		auto& we = (vel::WindowResizeEvent&)event;
		vel::Renderer::OnWindowResize(we.GetWidth(), we.GetHeight());
		m_EditorCamera.SetViewportSize(we.GetWidth(), we.GetHeight());
	}
}

physics::iRigidBody* PhysicsScene::CreateBall(glm::vec3 position, float size,float radius, std::string color)
{
	GameObject* gameObejct = new GameObject();
	gameObejct->name = "ball";
	gameObejct->transform.position = position;
	gameObejct->transform.rotation = glm::quat(glm::vec3(0.f));
	gameObejct->transform.scale = glm::vec3(radius);

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
	m_PhysicsWorld->AddBody(gameObejct->rigidBody);
	controlBall = gameObejct->rigidBody;
	return gameObejct->rigidBody;
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
	physics::iShape* planeShape = new physics::PlaneShape(0.0f, glm::vec3(0.f, 1.f, 0.f));

	// Spheres were falling through because of the computed plane
	/*physics::iShape* planeShape = ComputePlane(
									glm::vec3(-40.f, 0.f, -40.f),
									glm::vec3(40.f, 0.f, -40.f),
									glm::vec3(0.f, 0.f, 0.f)
								);*/
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
		gameObejct->transform.position = glm::vec3(0.f, 40.f, 0.f);
		gameObejct->transform.rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1, 0, 0));
		gameObejct->transform.scale = glm::vec3(40.f, 40.f, 1.f);
		gameObejct->m_Texture = vel::Texture2D::Create("assets/textures/ground.png");
		physics::RigidBodyDesc desc;
		physics::iShape* planeShape = ComputePlane(
			glm::vec3(-40.f, 40.f, -40.f),
			glm::vec3(40.f, 40.f, -40.f),
			glm::vec3(40.f, 40.f, 40.f)
		);
		desc.isStatic = true;
		desc.mass = 0;
		desc.position = glm::vec3(0.f, 40.f, 0.f);
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

void PhysicsScene::DetectChangeInControllBall()
{
	if (vel::Input::IsKeyPressed(KeyCode::D1))
		controlBall = ball1;
	if (vel::Input::IsKeyPressed(KeyCode::D2))
		controlBall = ball2;
	if (vel::Input::IsKeyPressed(KeyCode::D3))
		controlBall = ball3;
	if (vel::Input::IsKeyPressed(KeyCode::D4))
		controlBall = ball4;
	if (vel::Input::IsKeyPressed(KeyCode::D5))
		controlBall = ball5;
}

void PhysicsScene::HandleCamera()
{
	if (vel::Input::IsKeyPressed(KeyCode::Down))
	{
		if (FOV < 0.9f)
		{
			FOV += 0.001f;
		}
	}
	if (vel::Input::IsKeyPressed(KeyCode::Up))
	{
		if (FOV > 0.2f)
		{
			FOV -= 0.001f;
		}
	}

	if (vel::Input::IsKeyPressed(KeyCode::Left))
	{
		glm::mat4 rotationMat(1);
		rotationMat = glm::rotate(rotationMat, -0.001f, glm::vec3(0.0, 1.0, 0.0));
		glm::vec3 rotated = glm::vec3(rotationMat * glm::vec4(CameraPosition, 1.0));

		CameraPosition = rotated;
	}
	if (vel::Input::IsKeyPressed(KeyCode::Right))
	{
		glm::mat4 rotationMat(1);
		rotationMat = glm::rotate(rotationMat, 0.001f, glm::vec3(0.0, 1.0, 0.0));
		glm::vec3 rotated = glm::vec3(rotationMat * glm::vec4(CameraPosition, 1.0));

		CameraPosition = rotated;
	}
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

std::string PhysicsScene::CurrentlySelectedBall()
{
	if (controlBall == ball1)
	{
		return ball1info;
	}
	if (controlBall == ball2)
	{
		return ball2info;
	}
	if (controlBall == ball3)
	{
		return ball3info;
	}
	if (controlBall == ball4)
	{
		return ball4info;
	}
	if (controlBall == ball5)
	{
		return ball5info;
	}
}

Transform::Transform()
{
	scale = glm::vec3(1.f);
	rotation = glm::quat(glm::vec3(0.f));
	position = glm::vec3(0.f);
}
