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

	float floor(float value)
	{
		return (float)(uint32)value;
	}

	float frac(float value)
	{
		return value - floor(value);
	}

	Color Texture::Sampler2D_low(const vmath::vec2* uv) const
	{
		uint32 u = (uint32)(m_width * (*uv)[0]);
		uint32 v = (uint32)(m_height * (*uv)[1]);
		if (u >= m_width)
			u = m_width - 1;
		if (v >= m_height)
			v = m_height - 1;
		return Color(m_data[u + m_width * v]);
	}

	Color Texture::Sampler2D(const vmath::vec2* uv) const
	{
		vmath::vec2 uv_offset = *uv + vmath::vec2(0.5 / (float)m_width, 0.5 / (float)m_height);
		vmath::uvec2 uv0, uv1, uv2, uv3;
		vmath::vec2 uv_clamp = vmath::clamp(uv_offset, vmath::vec2(0), vmath::vec2(1));
		uv0[0] = (uint32)((m_width - 1) * uv_clamp[0]);
		uv0[1] = (uint32)((m_height - 1) * uv_clamp[1]);

		float fu = ((m_width-1) * uv_clamp[0]);
		float fv = ((m_height-1) * uv_clamp[1]);
		float a = frac(fu);
		float b = frac(fv);
		if (a > 0.5f)
		{
			uv1[0] = uv0[0] + 1;
			uv1[1] = uv0[1];
		}
		else
		{
			uv1[0] = uv0[0] > 0 ? uv0[0] - 1 : uv0[0];
			uv1[1] = uv0[1];
		}
		if (b > 0.5)
		{
			uv2[0] = uv0[0];
			uv2[1] = uv0[1] + 1;
			uv3[0] = uv1[0];
			uv3[1] = uv1[1] + 1;
		}
		else
		{
			uv2[0] = uv0[0];
			uv2[1] = uv0[1] > 0 ? uv0[1] - 1 : uv0[1];
			uv3[0] = uv1[0];
			uv3[1] = uv1[1] > 0 ? uv1[1] - 1 : uv1[1];
		}
		Color c[4];
		c[0] = m_data[uv0[0] + m_width * uv0[1]];
		c[1] = m_data[uv1[0] + m_width * uv1[1]];
		c[2] = m_data[uv2[0] + m_width * uv2[1]];
		c[3] = m_data[uv3[0] + m_width * uv3[1]];
		if (a > 0.5f)
		{
			c[0] = vmath::lerp(c[0], c[1], a - 0.5f);
			c[2] = vmath::lerp(c[2], c[3], a - 0.5f);
		}
		else
		{
			c[0] = vmath::lerp(c[1], c[0], a + 0.5f);
			c[2] = vmath::lerp(c[3], c[2], a + 0.5f);
		}
		if (b > 0.5f)
		{
			c[0] = vmath::lerp(c[0], c[2], b - 0.5f);
		}
		else
		{
			c[0] = vmath::lerp(c[2], c[0], b + 0.5f);
		}
		return c[0];
	}
}