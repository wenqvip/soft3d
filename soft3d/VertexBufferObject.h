#pragma once

namespace soft3d
{

	class VertexBufferObject
	{
	public:
		VertexBufferObject();
		~VertexBufferObject();

		void CopyVertexBuffer(const void* buffer, uint32_t size);
		void CopyIndexBuffer(const void* buffer, uint32_t size);
		void CopyColorBuffer(const void* buffer, uint32_t size);
		void CopyUVBuffer(const void* buffer, uint32_t size);
		void CopyNormalBuffer(const void* buffer, uint32_t size);

		const vmath::vec4* GetPos(uint32_t index) const;
		const uint32_t* GetColor(uint32_t index) const;
		const vmath::vec3* GetNormal(uint32_t index) const;
		const vmath::vec2* GetUV(uint32_t index) const;

		inline uint32_t GetSize() const {
			if (m_indexBuffer == nullptr)
				return m_size;
			else
				return m_indexSize;
		}

		inline bool useIndex() {
			return m_indexBuffer != nullptr;
		}
		uint32_t GetIndex(uint32_t index);

		inline bool hasUV() {
			return m_uvBuffer != nullptr;
		}


		enum RENDER_MODE
		{
			RENDER_LINE,
			RENDER_TRIANGLE,
		};

		enum CULL_MODE
		{
			CULL_CCW,
			CULL_CW,
			CULL_NONE,
		};

		enum UNIFORM_RELATIVE
		{
			MAX_UNIFORM_COUNT = 16,
		};

	public:
		RENDER_MODE m_mode;
		CULL_MODE m_cullMode;

	private:
		vmath::vec4* m_vertexBuffer;
		uint32_t m_size;

		uint32_t* m_colorBuffer;

		uint32_t* m_indexBuffer;
		uint32_t m_indexSize;

		vmath::vec2* m_uvBuffer;

		vmath::vec3* m_normalBuffer;
	};

}

