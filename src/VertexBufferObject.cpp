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

		m_mode = RENDER_TRIANGLE;
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

	void VertexBufferObject::CopyVertexBuffer(const void* buffer, uint32_t size)
	{
		if (m_vertexBuffer != nullptr)
			delete[] m_vertexBuffer;
		m_size = size / 4;
		m_vertexBuffer = new vec4[m_size];
		memcpy(m_vertexBuffer, buffer, m_size * 4 * sizeof(float));
	}

	const vec4* VertexBufferObject::GetPos(uint32_t index) const
	{
		if (m_vertexBuffer == nullptr || index >= m_size)
			return nullptr;
		return &(m_vertexBuffer[index]);
	}

	void VertexBufferObject::CopyColorBuffer(const void* buffer, uint32_t size)
	{
		if (m_colorBuffer != nullptr)
			delete[] m_colorBuffer;
		m_colorBuffer = new uint32_t[size];
		memcpy(m_colorBuffer, buffer, size * sizeof(uint32_t));
	}

	const uint32_t* VertexBufferObject::GetColor(uint32_t index) const
	{
		if (m_colorBuffer == nullptr || index >= m_size)
			return nullptr;
		return (uint32_t*)&(m_colorBuffer[index]);
	}

	void VertexBufferObject::CopyIndexBuffer(const void* buffer, uint32_t size)
	{
		if (m_indexBuffer != nullptr)
			delete[] m_indexBuffer;
		m_indexSize = size;
		m_indexBuffer = new uint32_t[size];
		memcpy(m_indexBuffer, buffer, size * sizeof(uint32_t));
	}

	uint32_t VertexBufferObject::GetIndex(uint32_t index)
	{
		if (m_indexBuffer == nullptr || index < m_indexSize)
			return m_indexBuffer[index];
		else
			return 0xffffffff;
	}

	void VertexBufferObject::CopyUVBuffer(const float* buffer, uint32_t size)
	{
		if (m_uvBuffer != nullptr)
			delete[] m_uvBuffer;
		uint32_t uvsize = size / 2;
		m_uvBuffer = new vec2[uvsize];
		memcpy(m_uvBuffer, buffer, uvsize * 2 * sizeof(float));
	}

	const vec2* VertexBufferObject::GetUV(uint32_t index) const
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

	void VertexBufferObject::CopyNormalBuffer(const void* buffer, uint32_t size)
	{
		if (m_normalBuffer != nullptr)
			delete[] m_normalBuffer;
		size /= 3;
		m_normalBuffer = new vec3[size];
		memcpy(m_normalBuffer, buffer, size * 3 * sizeof(float));
	}

	const vec3* VertexBufferObject::GetNormal(uint32_t index) const
	{
		if (m_normalBuffer == nullptr || index >= m_indexSize)
			return nullptr;
		return &(m_normalBuffer[index]);
	}
}