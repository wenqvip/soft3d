#pragma once
#include <memory>
#include "vmath.h"

namespace soft3d
{

	typedef unsigned short uint16;
	typedef unsigned int uint32;

	inline void uC2fC(uint32 color, vmath::vec4* colorf)
	{
		(*colorf)[0] = ((color & 0xff0000) >> 16) / 255.0f;
		(*colorf)[1] = ((color & 0xff00) >> 8) / 255.0f;
		(*colorf)[2] = (color & 0xff) / 255.0f;
		(*colorf)[3] = (color & 0xff000000) / 255.0f;
	}

	inline uint32 fC2uC(const vmath::vec4& colorf)
	{
		//return 0xfff;
		uint32 color = 0;
		color += (uint32)(0xff * colorf[2]) << 16;
		color += (uint32)(0xff * colorf[1]) << 8;
		color += (uint32)(0xff * colorf[0]);
		color += (uint32)(0xf0) << 24;

		return color;
	}

	inline uint32 colorMultiFloat(uint32 color, float ratio)
	{
		uint32 cc = 0;
		cc += uint32((color & 0xff0000) * ratio) & 0xff0000;
		cc += uint32((color & 0xff00) * ratio) & 0xff00;
		cc += uint32((color & 0xff) * ratio) & 0xff;
		return cc;
	}

	struct Color
	{
		unsigned char B;
		unsigned char G;
		unsigned char R;
		unsigned char A;
	};

	Color operator*(const Color& lf, float ratio)
	{
		Color cc;
		cc.B = lf.B * ratio;
		cc.G = lf.G * ratio;
		cc.R = lf.R * ratio;
		cc.A = lf.A * ratio;
		return cc;
	}

}
#include "Soft3dPipeline.h"