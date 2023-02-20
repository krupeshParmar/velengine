#pragma once

// For applications only

#include "vel/Application.h"
#include "vel/Layer.h"
#include "vel/Log.h"

#include "vel/ImGui/ImGuiLayer.h"

#include "vel/Core/Timestep.h"

// Temporary
#include "Platform/OpenGL/OpenGLShader.h"

// --------Input--------
#include "vel/Input.h"
#include "vel/KeyCodes.h"
// -----------------------------

// --------Physics--------Libraries
#include <iPhysicsFactory.h>
// -----------------------------


// --------Renderer----------
#include "vel/Renderer/Renderer.h"
#include "vel/Renderer/RenderCommand.h"

#include "vel/Renderer/Buffer.h"
#include "vel/Renderer/Shader.h"
#include "vel/Renderer/Texture.h"
#include "vel/Renderer/VertexArray.h"

#include "vel/Editor/EditorCamera.h"
// -----------------------------

// --------Entry Point ---------
#include "vel/EntryPoint.h"
// -----------------------------