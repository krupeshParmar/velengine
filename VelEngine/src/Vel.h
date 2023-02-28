#pragma once

// For applications only

#include "vel/Core/Application.h"
#include "vel/Core/Layer.h"
#include "vel/Core/Log.h"

#include "vel/ImGui/ImGuiLayer.h"

#include "vel/Core/Timestep.h"

// Temporary
#include "Platform/OpenGL/OpenGLShader.h"

// --------Input--------
#include "vel/Core/Input.h"
#include "vel/Core/KeyCodes.h"
// -----------------------------

// --------Physics--------Libraries
#include <iPhysicsFactory.h>
// -----------------------------


// --------Renderer----------
#include "vel/Renderer/Renderer.h"
#include "vel/Renderer/RenderCommand.h"

#include "vel/Renderer/Buffer.h"
#include "vel/Renderer/Shader.h"
#include "vel/Renderer/FrameBuffer.h"
#include "vel/Renderer/Texture.h"
#include "vel/Renderer/VertexArray.h"
#include "vel/Renderer/Model.h"

#include "vel/Editor/EditorCamera.h"
// -----------------------------


// --------Scene----------
#include "vel/Scene/Entity.h"
#include "vel/Scene/Component.h"
#include "vel/Scene/EntityManager.h"
#include "vel/Scene/LightManager.h"
#include "vel/Scene/SaveSystem.h"
// --------------------------
