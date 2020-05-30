#pragma once

namespace soft3d
{

	class Texture
	{
	public:
		Texture();
		~Texture();
		void CopyFromBuffer(const uint32_t* buf, int width, int height);
		Color Sampler2D(const vmath::vec2* uv) const;

		enum FILTER_MODE
		{
			NEAREST,
			BILINEAR,
		};
		FILTER_MODE filter_mode = BILINEAR;

	private:
		uint32_t* m_data;
		uint16_t m_width;
		uint16_t m_height;

		Color Sampler2D_nearest(const vmath::vec2* uv) const;
		Color Sampler2D_bilinear(const vmath::vec2* uv) const;
	};

}