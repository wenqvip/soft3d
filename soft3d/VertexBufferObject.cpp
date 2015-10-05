#include "stdafx.h"
#include "VertexBufferObject.h"

using namespace vmath;

namespace soft3d
{

	VertexBufferObject::VertexBufferObject(uint32 stride)
	{
		m_buffer = nullptr;
		m_size = 0;
		m_stride = stride;

		m_posOffset = -1;
		m_colorOffset = -1;
		m_uvOffset = -1;
		m_normalOffset = -1;
	}


	VertexBufferObject::~VertexBufferObject()
	{
		delete[] m_buffer;
	}

	void VertexBufferObject::CopyFromBuffer(const void* buffer, uint32 size)
	{
		if (m_buffer != nullptr)
			delete[] m_buffer;
		m_size = size;
		m_buffer = new float[m_stride * size];
		memcpy(m_buffer, buffer, size * m_stride * sizeof(float));
	}

	const vec4* VertexBufferObject::GetPos(uint32 index) const
	{
		if (m_posOffset == -1)
			return nullptr;
		return (vec4*)&(m_buffer[m_stride * index + m_posOffset]);
	}

	const uint32* VertexBufferObject::GetColor(uint32 index) const
	{
		if (m_colorOffset == -1)
			return nullptr;
		return (uint32*)&(m_buffer[m_stride * index + m_colorOffset]);
	}

	const vec3* VertexBufferObject::GetNormal(uint32 index) const
	{
		if (m_normalOffset == -1)
			return nullptr;
		return (vec3*)&(m_buffer[m_stride * index + m_normalOffset]);
	}

	const vec2* VertexBufferObject::GetUV(uint32 index) const
	{
		if (m_uvOffset == -1)
			return nullptr;
		return (vec2*)&(m_buffer[m_stride * index + m_uvOffset]);
	}

}