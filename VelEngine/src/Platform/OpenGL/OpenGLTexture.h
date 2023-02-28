#pragma once
#include "vel/Renderer/Texture.h"
#include <glad/glad.h>

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
}

