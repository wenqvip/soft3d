#pragma once

namespace soft3d
{

	class Rasterizer
	{
	public:
		Rasterizer();
		virtual ~Rasterizer();

		void Fragment(uint32* out_color, uint32 src0, uint32 src1, float ratio);
		void Fragment(uint32* out_color, uint32 src0, uint32 src1, uint32 src2, float ratio0, float ratio1);
		void BresenhamLine(int x0, int y0, int x1, int y1, uint32 src0, uint32 src1);
		void Triangle(int x0, int y0, float z0, int x1, int y1, float z1, int x2, int y2, float z2, uint32 src0, uint32 src1, uint32 src2);

	protected:
		FragmentProcessor m_fp;
	};

}
