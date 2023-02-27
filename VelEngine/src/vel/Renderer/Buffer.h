#pragma once

namespace vel
{

	// Temporary

	struct Vertices
	{
		float r, g, b, a;		//	in vec4 vColour;			// Was vec3
		float x, y, z, w;		//	in vec4 vPosition;			// Was vec3
		float nx, ny, nz, nw;	//	in vec4 vNormal;			// Vertex normal
		float u0, v0, u1, v1;	//	in vec4 vUVx2;				// 2 x Texture coords
		float tx, ty, tz, tw;	//	in vec4 vTangent;			// For bump mapping
		float bx, by, bz, bw;	//	in vec4 vBiNormal;			// For bump mapping
		float vBoneID[4];		//	in vec4 vBoneID;		// For skinned mesh (FBX)
		float vBoneWeight[4];	//	in vec4 vBoneWeight;		// For skinned mesh (FBX)
	};

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case vel::ShaderDataType::Float:	return 4;
			case vel::ShaderDataType::Float2:	return 4 * 2;
			case vel::ShaderDataType::Float3:	return 4 * 3;
			case vel::ShaderDataType::Float4:	return 4 * 4;
			case vel::ShaderDataType::Mat3:		return 4 * 3 * 3;
			case vel::ShaderDataType::Mat4:		return 4 * 4 * 4;
			case vel::ShaderDataType::Int:		return 4;
			case vel::ShaderDataType::Int2:		return 4 * 2;
			case vel::ShaderDataType::Int3:		return 4 * 3;
			case vel::ShaderDataType::Int4:		return 4 * 4;
			case vel::ShaderDataType::Bool:		return 1;
		}
		VEL_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Offset;
		uint32_t Size;
		bool Normalized;

		BufferElement() {}

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case vel::ShaderDataType::Float:	return 1;
				case vel::ShaderDataType::Float2:	return 2;
				case vel::ShaderDataType::Float3:	return 3;
				case vel::ShaderDataType::Float4:	return 4;
				case vel::ShaderDataType::Mat3:		return 3 * 3;
				case vel::ShaderDataType::Mat4:		return 4 * 4;
				case vel::ShaderDataType::Int:		return 1;
				case vel::ShaderDataType::Int2:		return 2;
				case vel::ShaderDataType::Int3:		return 3;
				case vel::ShaderDataType::Int4:		return 4;
				case vel::ShaderDataType::Bool:		return 1;
			}
			VEL_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			:m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		inline uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const{ return m_Elements.end(); }
	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(void* vertices, uint32_t size);

	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};


}
