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
	physics::iRigidBody* CreateBall(glm::vec3 position, float size, float radius, std::string color);
	void CreateGround();
	void CreateWalls();
	void LoadCoordinates();
	void DetectChangeInControllBall();
	void HandleCamera();
	bool LoadPlyFiles(std::string filename, GameObject* gameObject);

private:
	physics::iRigidBody* ball1;
	physics::iRigidBody* ball2;
	physics::iRigidBody* ball3;
	physics::iRigidBody* ball4;
	physics::iRigidBody* ball5;
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

};