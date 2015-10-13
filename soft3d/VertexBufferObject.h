#pragma once

namespace soft3d
{

	class VertexBufferObject
	{
	public:
		VertexBufferObject();
		~VertexBufferObject();

		void CopyVertexBuffer(const void* buffer, uint32 size);
		void CopyIndexBuffer(const void* buffer, uint32 size);
		void CopyColorBuffer(const void* buffer, uint32 size);
		void CopyUVBuffer(const void* buffer, uint32 size);
		void CopyNormalBuffer(const void* buffer, uint32 size);

		const vmath::vec4* GetPos(uint32 index) const;
		const uint32* GetColor(uint32 index) const;
		const vmath::vec3* GetNormal(uint32 index) const;
		const vmath::vec2* GetUV(uint32 index) const;

		inline uint32 GetSize() const {
			if (m_indexBuffer == nullptr)
				return m_size;
			else
				return m_indexSize;
		}

		inline bool useIndex() {
			return m_indexBuffer != nullptr;
		}
		uint32 GetIndex(uint32 index);

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

	public:
		vmath::mat4 mv_matrix;
		vmath::mat4 proj_matrix;

		RENDER_MODE m_mode;
		CULL_MODE m_cullMode;

	private:
		vmath::vec4* m_vertexBuffer;
		uint32 m_size;

		uint32* m_colorBuffer;

		uint32* m_indexBuffer;
		uint32 m_indexSize;

		vmath::vec2* m_uvBuffer;

		vmath::vec3* m_normalBuffer;
	};

}

