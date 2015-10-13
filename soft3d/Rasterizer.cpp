#include "soft3d.h"
#include "FragmentProcessor.h"
#include "Rasterizer.h"
#include <stdlib.h>

using namespace vmath;

namespace soft3d
{

	Rasterizer::Rasterizer(uint16 width, uint16 height)
	{
		m_width = width;
		m_height = height;
		m_frameBuffer = new uint32[width*height*sizeof(uint32)];
	}


	Rasterizer::~Rasterizer()
	{
		delete m_frameBuffer;
	}

	uint32* Rasterizer::GetFBPixelPtr(uint16 x, uint16 y)
	{
		y = 599 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return nullptr;

		int index = y * m_width + x;
		if (index >= (uint32)m_width * m_height)
			return nullptr;
		return &(m_frameBuffer[index]);
	}

	int Rasterizer::DrawPixel(uint16 x, uint16 y, uint32 color, uint16 size)
	{
		y = 599 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return -1;
		if (size > 100 || size < 1)
			size = 1;

		for (uint16 i = 0; i < size; i++)
		{
			for (uint16 j = 0; j < size; j++)
				SetFrameBuffer((y + j - size / 2) * m_width + x + i - size / 2, color);
		}
		return 0;
	}

	void Rasterizer::SetFrameBuffer(uint32 index, uint32 value)
	{
		if (index >= (uint32)m_width * m_height)
			return;
		m_frameBuffer[index] = value;
	}

	int Rasterizer::Clear(uint32 color)
	{
		memset(m_frameBuffer, color, m_width * m_height * sizeof(uint32));
		return 0;
	}

	void Rasterizer::Fragment(const PipeLineData* pipelineData, uint32 x, uint32 y, uint32 src0, uint32 src1, float ratio)
	{
		Color* colorVsOut = pipelineData->color;
		vec2* uv = pipelineData->uv;
		float* rhw = pipelineData->rhw;
		float ratio1 = 1.0f - ratio;

		//in
		m_fp.color = colorVsOut[src0] * ratio + colorVsOut[src1] * (1.0f - ratio);
		m_fp.uv[0] = uv[src0][0] * ratio + uv[src1][0] * (1.0f - ratio);
		m_fp.uv[1] = uv[src0][1] * ratio + uv[src1][1] * (1.0f - ratio);
		m_fp.tex = Soft3dPipeline::Instance()->CurrentTex();

		float fp_rhw = rhw[src0] * ratio + rhw[src1] * ratio1;//也要对rhw做插值
		m_fp.uv /= fp_rhw;//把w乘回来，这样就可能得到正确的uv了，不知道为什么

		//out
		m_fp.out_color = GetFBPixelPtr(x, y);
		if (m_fp.out_color == nullptr)
			return;
		m_fp.Process();
	}

	void Rasterizer::Fragment(const PipeLineData* pipelineData, uint32 x, uint32 y, uint32 src0, uint32 src1, uint32 src2, float ratio0, float ratio1)
	{
		Color* colorVsOut = pipelineData->color;
		vec2* uv = pipelineData->uv;
		float* rhw = pipelineData->rhw;
		float ratio2 = 1.0f - ratio0 - ratio1;
		
		//in
		m_fp.color = colorVsOut[src0] * ratio0 + colorVsOut[src1] * ratio1 + colorVsOut[src2] * ratio2;
		m_fp.uv[0] = uv[src0][0] * ratio0 + uv[src1][0] * ratio1 + uv[src2][0] * ratio2;
		m_fp.uv[1] = uv[src0][1] * ratio0 + uv[src1][1] * ratio1 + uv[src2][1] * ratio2;
		float fp_rhw = rhw[src0] * ratio0 + rhw[src1] * ratio1 + rhw[src2] * ratio2;//也要对rhw做插值
		m_fp.uv /= fp_rhw;//把w乘回来，这样就可能得到正确的uv了，不知道为什么
		m_fp.tex = Soft3dPipeline::Instance()->CurrentTex();

		//out
		m_fp.out_color = GetFBPixelPtr(x, y);
		if (m_fp.out_color == nullptr)
			return;
		m_fp.Process();
	}

	void Rasterizer::BresenhamLine(const PipeLineData* pipelineData, uint32 src0, uint32 src1)
	{
		int x0 = pipelineData->pos[src0][0];
		int y0 = pipelineData->pos[src0][1];
		int x1 = pipelineData->pos[src1][0];
		int y1 = pipelineData->pos[src1][1];

		if (x0 < 0 || x0 > m_width)
			return;
		if (x1 < 0 || x1 > m_width)
			return;
		if (y0 < 0 || y0 > m_height)
			return;
		if (y1 < 0 || y1 > m_height)
			return;

		DrawPixel(x0, y0, pipelineData->color[src0], 5);

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
				Fragment(pipelineData, x, y, src0, src1, ratio);
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
				Fragment(pipelineData, x, y, src0, src1, ratio);
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

	void Rasterizer::Triangle(const PipeLineData* pipelineData, uint32 src0, uint32 src1, uint32 src2)
	{
		float fx1 = pipelineData->pos[src0][0] + 0.5f;
		float fy1 = pipelineData->pos[src0][1] + 0.5f;
		float fx2 = pipelineData->pos[src1][0] + 0.5f;
		float fy2 = pipelineData->pos[src1][1] + 0.5f;
		float fx3 = pipelineData->pos[src2][0] + 0.5f;
		float fy3 = pipelineData->pos[src2][1] + 0.5f;

		int x1 = fx1;
		int x2 = fx2;
		int x3 = fx3;
		int y1 = fy1;
		int y2 = fy2;
		int y3 = fy3;

		float Dx12 = fx1 - fx2;
		float Dx23 = fx2 - fx3;
		float Dx31 = fx3 - fx1;

		float Dy12 = fy1 - fy2;
		float Dy23 = fy2 - fy3;
		float Dy31 = fy3 - fy1;

		int minx = vmath::min<int>(fx1, fx2, fx3);
		int maxx = vmath::max<int>(fx1, fx2, fx3);
		int miny = vmath::min<int>(fy1, fy2, fy3);
		int maxy = vmath::max<int>(fy1, fy2, fy3);

		float C1 = Dy12 * fx1 - Dx12 * fy1;
		float C2 = Dy23 * fx2 - Dx23 * fy2;
		float C3 = Dy31 * fx3 - Dx31 * fy3;

		float Cy1 = C1 + Dx12 * miny - Dy12 * minx;
		float Cy2 = C2 + Dx23 * miny - Dy23 * minx;
		float Cy3 = C3 + Dx31 * miny - Dy31 * minx;

		for (int y = miny; y <= maxy; y++)
		{
			float Cx1 = Cy1;
			float Cx2 = Cy2;
			float Cx3 = Cy3;
			for (int x = minx; x <= maxx; x++)
			{
				if (Cx1 <= 0 && Cx2 <= 0 && Cx3 <= 0)
				{
					float ratio1 = ((y - y3)*(x1 - x3) - (y1 - y3)*(x - x3)) / (float)((y2 - y3)*(x1 - x3) - (y1 - y3)*(x2 - x3));
					float ratio0 = ((y - y3)*(x2 - x3) - (x - x3)*(y2 - y3)) / (float)((y1 - y3)*(x2 - x3) - (x1 - x3)*(y2 - y3));
					Fragment(pipelineData, x, y, src0, src1, src2, ratio0, ratio1);
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