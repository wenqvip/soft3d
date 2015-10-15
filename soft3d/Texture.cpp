#include "soft3d.h"
#include "Texture.h"

namespace soft3d
{

	Texture::Texture()
	{
		m_data = nullptr;
	}


	Texture::~Texture()
	{
	}


	void Texture::CopyFromBuffer(const uint32* buf, int width, int height)
	{
		m_data = new uint32[width * height];
		memcpy(m_data, buf, width * height * sizeof(uint32));
		m_width = width;
		m_height = height;
	}


	Color Texture::Sampler2D(const vmath::vec2* uv) const
	{
		uint32 u = (uint32)(m_width * (*uv)[0]);
		uint32 v = (uint32)(m_height * (*uv)[1]);
		if (u >= m_width)
			u = m_width - 1;
		if (v >= m_height)
			v = m_height - 1;
		return Color(m_data[u + m_width * v]);
	}
}