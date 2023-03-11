#pragma once
#include "vel/Renderer/Texture.h"
#include <vector>
#include <glad/glad.h>
#include <vel/Renderer/Shader.h>
#include <glm/glm.hpp>

namespace vel {
    class OpenGLTexture2D :
        public Texture2D
    {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(const std::string& path);
        virtual ~OpenGLTexture2D();

        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(void* data, uint32_t size) override;
        virtual void CreateData(const std::string& path) override;
        virtual void RemoveData() override;

        virtual void CreateData(const std::vector<std::string>) override {};
        virtual void DrawSkyBox(glm::mat4 viewProjection, Ref<Shader> shader, Ref<VertexArray> vertexArray) override {};

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }

        virtual std::string GetPath() const override { return m_Path.substr(m_Path.find_last_of('/') + 1, m_Path.size()); }
        virtual void Bind(uint32_t slot = 0) const override;
        virtual bool IsLoaded() const override { return m_Loaded; }

    private:
        bool m_Loaded = false;
        std::string m_Path;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;
    };

    class OpenGLTextureCubeMap : public TextureCubeMap
    {
    public:
        OpenGLTextureCubeMap(const std::vector<std::string> paths);
        virtual ~OpenGLTextureCubeMap();

        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(void* data, uint32_t size) override {};
        virtual void CreateData(const std::string& path) override {};

        virtual void CreateData(const std::vector<std::string>) override;
        virtual void RemoveData() override;

        virtual void DrawSkyBox(glm::mat4 modelviewprojection, Ref<Shader> shader, Ref<VertexArray> vertexArray) override;

        virtual uint32_t GetWidth() const override { return 0; }
        virtual uint32_t GetHeight() const override { return 0; }

        virtual std::string GetPath() const override { return ""; }
        virtual void Bind(uint32_t slot = 0) const override;
        virtual bool IsLoaded() const override { return m_Loaded; }

    private:
        bool m_Loaded = false;
        std::vector<std::string> m_Paths;
        uint32_t m_RendererID;
        glm::vec3 tilingfactor = glm::vec3(1.f);
    };
}

