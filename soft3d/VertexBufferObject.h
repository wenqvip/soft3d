#pragma once
#include "soft3d.h"
#include "vmath.h"

namespace soft3d
{

	class VertexBufferObject
	{
	public:
		VertexBufferObject(uint32 stride);
		~VertexBufferObject();

		void CopyFromBuffer(const void* buffer, uint32 size);
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

		void CopyIndexBuffer(const void* buffer, uint32 size);
		inline bool useIndex() {
			return m_indexBuffer != nullptr;
		}
		uint32 GetIndex(uint32 index);

	public:
		vmath::mat4 mv_matrix;
		vmath::mat4 proj_matrix;

		int m_posOffset;
		int m_colorOffset;
		int m_normalOffset;
		int m_uvOffset;

	private:
		float* m_buffer;
		uint32 m_size;
		uint32 m_stride;

		uint32* m_indexBuffer;
		uint32 m_indexSize;
	};

}

