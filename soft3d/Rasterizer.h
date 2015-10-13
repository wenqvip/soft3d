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

		void Fragment(const PipeLineData* pipelineData, uint32 x, uint32 y, uint32 src0, uint32 src1, float ratio);
		void Fragment(const PipeLineData* pipelineData, uint32 x, uint32 y, uint32 src0, uint32 src1, uint32 src2, float ratio0, float ratio1);
		void BresenhamLine(const PipeLineData* pipelineData, uint32 src0, uint32 src1);
		void Triangle(const PipeLineData* pipelineData, uint32 src0, uint32 src1, uint32 src2);

		const uint32* GetFrameBuffer() {
			return m_frameBuffer;
		}

	protected:
		void SetFrameBuffer(uint32 index, uint32 value);

	protected:
		FragmentProcessor m_fp;

		uint16 m_width;
		uint16 m_height;
		uint32* m_frameBuffer;
	};

}
