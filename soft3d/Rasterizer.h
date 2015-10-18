#pragma once

namespace soft3d
{
	struct PipeLineData;

	class Rasterizer
	{
	public:
		Rasterizer(uint16 width, uint16 height);
		virtual ~Rasterizer();

		int Clear(uint32 color);
		int DrawPixel(uint16 x, uint16 y, uint32 color, uint16 size = 1);
		uint32* GetFBPixelPtr(uint16 x, uint16 y);

		void Fragment(const VS_OUT* vo0, const VS_OUT* vo1, uint32 x, uint32 y, float ratio);
		void Fragment(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, uint32 x, uint32 y, float ratio0, float ratio1);
		void BresenhamLine(const VS_OUT* vo0, const VS_OUT* vo1);
		void Triangle(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2);

		const uint32* GetFrameBuffer() {
			return m_frameBuffer;
		}

	protected:
		void SetFrameBuffer(uint32 index, uint32 value);
		void SetZBufferV(uint32 x, uint32 y, float value);
		float GetZBufferV(uint32 x, uint32 y);

	protected:
		FragmentProcessor m_fp;

		uint16 m_width;
		uint16 m_height;
		uint32* m_frameBuffer;
		float* m_zBuffer;
	};

}
