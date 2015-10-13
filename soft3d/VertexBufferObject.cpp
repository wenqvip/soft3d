#include "soft3d.h"
#include "vmath.h"
#include "VertexBufferObject.h"

using namespace vmath;

namespace soft3d
{

	VertexBufferObject::VertexBufferObject()
	{
		m_vertexBuffer = nullptr;
		m_size = 0;

		m_colorBuffer = nullptr;

		m_indexBuffer = nullptr;
		m_indexSize = 0;

		m_uvBuffer = nullptr;

		m_normalBuffer = nullptr;

		m_mode = RENDER_LINE;
		m_cullMode = CULL_CCW;
	}


	VertexBufferObject::~VertexBufferObject()
	{
		if (m_vertexBuffer != nullptr)
			delete[] m_vertexBuffer;
		if (m_colorBuffer != nullptr)
			delete[] m_colorBuffer;
		if (m_indexBuffer != nullptr)
			delete[] m_indexBuffer;
		if (m_uvBuffer != nullptr)
			delete[] m_uvBuffer;
		if (m_normalBuffer != nullptr)
			delete[] m_normalBuffer;
	}

	void VertexBufferObject::CopyVertexBuffer(const void* buffer, uint32 size)
	{
		if (m_vertexBuffer != nullptr)
			delete[] m_vertexBuffer;
		m_size = size / 4;
		m_vertexBuffer = new vec4[m_size];
		memcpy(m_vertexBuffer, buffer, m_size * 4 * sizeof(float));
	}

	const vec4* VertexBufferObject::GetPos(uint32 index) const
	{
		if (m_vertexBuffer == nullptr || index >= m_size)
			return nullptr;
		return &(m_vertexBuffer[index]);
	}

	void VertexBufferObject::CopyColorBuffer(const void* buffer, uint32 size)
	{
		if (m_colorBuffer != nullptr)
			delete[] m_colorBuffer;
		m_colorBuffer = new uint32[size];
		memcpy(m_colorBuffer, buffer, size * sizeof(uint32));
	}

	const uint32* VertexBufferObject::GetColor(uint32 index) const
	{
		if (m_colorBuffer == nullptr || index >= m_size)
			return nullptr;
		return (uint32*)&(m_colorBuffer[index]);
	}

	void VertexBufferObject::CopyIndexBuffer(const void* buffer, uint32 size)
	{
		if (m_indexBuffer != nullptr)
			delete[] m_indexBuffer;
		m_indexSize = size;
		m_indexBuffer = new uint32[size];
		memcpy(m_indexBuffer, buffer, size * sizeof(uint32));
	}

	uint32 VertexBufferObject::GetIndex(uint32 index)
	{
		if (m_indexBuffer == nullptr || index < m_indexSize)
			return m_indexBuffer[index];
		else
			return 0xffffffff;
	}

	void VertexBufferObject::CopyUVBuffer(const void* buffer, uint32 size)
	{
		if (m_uvBuffer != nullptr)
			delete[] m_uvBuffer;
		uint32 uvsize = size / 2;
		m_uvBuffer = new vec2[uvsize];
		memcpy(m_uvBuffer, buffer, uvsize * 2 * sizeof(void*));
	}

	const vec2* VertexBufferObject::GetUV(uint32 index) const
	{
		if (m_indexBuffer != nullptr)
		{
			if (index >= m_indexSize)
				return nullptr;
		}
		else
		{
			if (index >= m_size)
				return nullptr;
		}
		if (m_uvBuffer == nullptr)
			return nullptr;
		return &(m_uvBuffer[index]);
	}

	void VertexBufferObject::CopyNormalBuffer(const void* buffer, uint32 size)
	{
		if (m_normalBuffer != nullptr)
			delete[] m_normalBuffer;
		size /= 3;
		m_normalBuffer = new vec3[size];
		memcpy(m_normalBuffer, buffer, size * 3 * sizeof(float));
	}

	const vec3* VertexBufferObject::GetNormal(uint32 index) const
	{
		if (m_normalBuffer == nullptr && index >= m_indexSize)
			return nullptr;
		return &(m_normalBuffer[index]);
	}
}