#pragma once
#include <Vel.h>
#include <iRigidBody.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Transform
{
	Transform();
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
};

struct GameObject
{
	Transform transform;
	std::string name;
	physics::iRigidBody* rigidBody;
	vel::Ref<vel::VertexArray> m_VertexArray;
	vel::Ref<vel::Texture2D> m_Texture;
};

class PhysicsScene : public vel::Layer
{
public:
	PhysicsScene();
	void OnUpdate(vel::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(vel::Event& event) override;

private:
	void CreateBall(glm::vec3 position, float size, float radius, std::string color);
	void CreateGround();
	void CreateWalls();
	void LoadCoordinates();
	bool LoadPlyFiles(std::string filename, GameObject* gameObject);

private:
	int m_SelectedBall = 0;
	std::vector<GameObject*> m_GameObjects;
	physics::iPhysicsFactory* m_PhysicsFactory;
	physics::iPhysicsWorld* m_PhysicsWorld;
	vel::ShaderLibrary m_ShaderLibrary;
	vel::Ref<vel::Shader> m_TextureShader;
	vel::EditorCamera m_EditorCamera;
	physics::iRigidBody* controlBall = nullptr;

	glm::vec3 CameraPosition = glm::vec3(0.f, 30.f, 120.f);
	float FOV = 0.6f;

	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::vec3(0.0f, 0.0f, 0.0f);

	int SCR_WIDTH = 1200;
	int SCR_HEIGHT = 800;

	float YAW = 90.0f;
	float PITCH = 0.0f;
	float SENSITIVITY = 0.1f;
	float xpos;
	float ypos;

	float deltaTime = 0.f;
	float lastFrame = 0.f;
	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;

};