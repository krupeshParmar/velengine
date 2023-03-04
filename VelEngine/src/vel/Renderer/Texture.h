#pragma once

#include "vel/Core/Core.h"
#include "Shader.h"
#include <vector>
#include <string>
#include "VertexArray.h"
namespace vel
{
	class Texture
	{
	public:
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual ~Texture() = default;

		virtual uint32_t GetRendererID() const = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void CreateData(const std::string& path) = 0;

		virtual void CreateData(const std::vector<std::string>) = 0;
		virtual void DrawSkyBox(glm::mat4 viewProjection, Ref<Shader> shader, Ref<VertexArray> vertexArray) = 0;

		virtual void RemoveData() = 0;

		virtual std::string GetPath() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual bool IsLoaded() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);
	};

	class TextureCubeMap : public Texture
	{
	public:
		static Ref<TextureCubeMap> Create(const std::vector<std::string> paths);
	};

}