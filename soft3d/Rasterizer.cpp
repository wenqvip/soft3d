#include "soft3d.h"
#include "FragmentProcessor.h"
#include "Rasterizer.h"
#include <stdlib.h>

using namespace vmath;

namespace soft3d
{

	Rasterizer::Rasterizer()
	{
	}


	Rasterizer::~Rasterizer()
	{
	}

	void Rasterizer::Fragment(uint32* out_color, uint32 src0, uint32 src1, float ratio)
	{
		uint32* colorVsOut = Soft3dPipeline::Instance()->GetVSOut()->color;
		m_fp.color = colorMultiFloat(colorVsOut[src0], ratio) + colorMultiFloat(colorVsOut[src1], (1.0f - ratio));
		m_fp.out_color = out_color;
		m_fp.Process();
	}

	void Rasterizer::Fragment(uint32* out_color, uint32 src0, uint32 src1, uint32 src2, float ratio0, float ratio1)
	{
		uint32* colorVsOut = Soft3dPipeline::Instance()->GetVSOut()->color;
		float ratio2 = 1.0f - ratio0 - ratio1;

		m_fp.color = colorMultiFloat(colorVsOut[src0], ratio0) + colorMultiFloat(colorVsOut[src1], ratio1) + colorMultiFloat(colorVsOut[src2], ratio2);
		m_fp.out_color = out_color;
		m_fp.Process();
	}

	void Rasterizer::BresenhamLine(int x0, int y0, int x1, int y1, uint32 src0, uint32 src1)
	{
		int x, y, dx, dy;
		dx = x1 - x0;
		dy = y1 - y0;
		x = x0;
		y = y0;
		if (abs(dy) <= abs(dx))
		{
			int e = -abs(dx);
			for (int i = 0; i <= abs(dx); i++)
			{
				float ratio = (x1 - x) / (float)dx;
				Fragment(Soft3dPipeline::Instance()->GetFBPixelPtr(x, y), src0, src1, ratio);
				x = dx > 0 ? x + 1 : x - 1;
				e += 2 * abs(dy);
				if (e >= 0)
				{
					y = dy > 0 ? y + 1 : y - 1;
					e -= 2 * abs(dx);
				}
			}
		}
		else
		{
			int e = -abs(dy);
			for (int i = 0; i <= abs(dy); i++)
			{
				float ratio = (y1 - y) / (float)dy;
				Fragment(Soft3dPipeline::Instance()->GetFBPixelPtr(x, y), src0, src1, ratio);
				y = dy > 0 ? y + 1 : y - 1;
				e += 2 * abs(dx);
				if (e >= 0)
				{
					x = dx > 0 ? x + 1 : x - 1;
					e -= 2 * abs(dy);
				}
			}
		}
	}

	void Rasterizer::Triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32 src0, uint32 src1, uint32 src2)
	{
		float fy1 = y0;
		float fy2 = y1;
		float fy3 = y2;

		float fx1 = x0;
		float fx2 = x1;
		float fx3 = x2;

		float Dx12 = fx1 - fx2;
		float Dx23 = fx2 - fx3;
		float Dx31 = fx3 - fx1;

		float Dy12 = fy1 - fy2;
		float Dy23 = fy2 - fy3;
		float Dy31 = fy3 - fy1;

		int minx = vmath::min<int>(x0, x1, x2);
		int maxx = vmath::max<int>(x0, x1, x2);
		int miny = vmath::min<int>(y0, y1, y2);
		int maxy = vmath::max<int>(y0, y1, y2);

		float C1 = Dy12 * fx1 - Dx12 * fy1;
		float C2 = Dy23 * fx2 - Dx23 * fy2;
		float C3 = Dy31 * fx3 - Dx31 * fy3;

		float Cy1 = C1 + Dx12 * miny - Dy12 * minx;
		float Cy2 = C2 + Dx23 * miny - Dy23 * minx;
		float Cy3 = C3 + Dx31 * miny - Dy31 * minx;

		for (int y = miny; y < maxy; y++)
		{
			float Cx1 = Cy1;
			float Cx2 = Cy2;
			float Cx3 = Cy3;
			for (int x = minx; x < maxx; x++)
			{
				if (Cx1 <= 0 && Cx2 <= 0 && Cx3 <= 0)
				{
					float ratio1 = ((y - y2)*(x0 - x2) - (y0 - y2)*(x - x2)) / (float)((y1 - y2)*(x0 - x2) - (y0 - y2)*(x1 - x2));
					float ratio0 = ((y - y2)*(x1 - x2) - (x - x2)*(y1 - y2)) / (float)((y0 - y2)*(x1 - x2) - (x0 - x2)*(y1 - y2));
					Fragment(Soft3dPipeline::Instance()->GetFBPixelPtr(x, y), src0, src1, src2, ratio0, ratio1);
					//Soft3dPipeline::Instance()->DrawPixel(x, y, 0xffffff);
				}
				Cx1 -= Dy12;
				Cx2 -= Dy23;
				Cx3 -= Dy31;
			}
			Cy1 += Dx12;
			Cy2 += Dx23;
			Cy3 += Dx31;
		}
	}

}