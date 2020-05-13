#include "soft3d.h"
#include "FragmentProcessor.h"
#include "VertexBufferObject.h"
#include <vector>
#include <stdlib.h>
#include <functional>
#include <mutex>

#include "Rasterizer.h"

using namespace vmath;

namespace soft3d
{
	uint32_t* Rasterizer::m_frameBuffer = nullptr;
	float* Rasterizer::m_zBuffer = nullptr;

	Rasterizer::Rasterizer(uint16_t width, uint16_t height)
		: m_workThread(std::bind(&Rasterizer::Rasterize, this))
	{
		m_width = width;
		m_height = height;
		if(m_frameBuffer == nullptr)
			m_frameBuffer = new uint32_t[width*height*sizeof(uint32_t)];
		if(m_zBuffer == nullptr)
			m_zBuffer = new float[width*height*sizeof(float)];
	}


	Rasterizer::~Rasterizer()
	{
		if (m_frameBuffer)
		{
			delete m_frameBuffer;
			m_frameBuffer = nullptr;
		}
		if (m_zBuffer)
		{
			delete m_zBuffer;
			m_zBuffer = nullptr;
		}
	}

	uint32_t* Rasterizer::GetFBPixelPtr(uint16_t x, uint16_t y)
	{
		y = 599 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return nullptr;

		int index = y * m_width + x;
		if (index >= (uint32_t)m_width * m_height)
			return nullptr;
		return &(m_frameBuffer[index]);
	}

	int Rasterizer::DrawPixel(uint16_t x, uint16_t y, uint32_t color, uint16_t size)
	{
		y = 599 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return -1;
		if (size > 100 || size < 1)
			size = 1;

		for (uint16_t i = 0; i < size; i++)
		{
			for (uint16_t j = 0; j < size; j++)
				SetFrameBuffer((y + j - size / 2) * m_width + x + i - size / 2, color);
		}
		return 0;
	}

	void Rasterizer::SetFrameBuffer(uint32_t index, uint32_t value)
	{
		if (index >= (uint32_t)m_width * m_height)
			return;
		m_frameBuffer[index] = value;
	}

	void Rasterizer::SetZBufferV(uint32_t x, uint32_t y, float value)
	{
		y = m_height - 1 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return;
		int index = y * m_width + x;
		m_zBuffer[index] = value;
	}

	float Rasterizer::GetZBufferV(uint32_t x, uint32_t y)
	{
		y = m_height - 1 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return 0.0f;
		int index = y * m_width + x;
		return m_zBuffer[index];
	}

	int Rasterizer::Clear(uint32_t color)
	{
		memset(m_frameBuffer, color, m_width * m_height * sizeof(uint32_t));
		memset(m_zBuffer, 0, m_width* m_height* sizeof(float));
		return 0;
	}

	void Rasterizer::Fragment(const VS_OUT* vo0, const VS_OUT* vo1, uint32_t x, uint32_t y, float ratio)
	{
		m_fp.fs_in.Interpolate(vo0, vo1, ratio, 1.0f - ratio);
		m_fp.fs_in.rhw += 0.001f;
		if (m_fp.fs_in.rhw < GetZBufferV(x, y))
			return;
		m_fp.tex = nullptr;// Soft3dPipeline::Instance()->CurrentTex();

		m_fp.out_color = GetFBPixelPtr(x, y);
		if (m_fp.out_color == nullptr)
			return;
		m_fp.fs_in.mode = VS_OUT::COLOR_MODE;
		m_fp.Process();
		SetZBufferV(x, y, m_fp.fs_in.rhw);
	}

	void Rasterizer::Fragment(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, uint32_t x, uint32_t y, float ratio0, float ratio1)
	{
		float ratio2 = 1.0f - ratio0 - ratio1;
		if (m_fp.fs_in.InterpolateRHW(vo0, vo1, vo2, ratio0, ratio1, ratio2) < GetZBufferV(x, y))
			return;

		m_fp.fs_in.Interpolate(vo0, vo1, vo2, ratio0, ratio1, ratio2);

		m_fp.tex = Soft3dPipeline::Instance()->CurrentTex();

		m_fp.out_color = GetFBPixelPtr(x, y);
		if (m_fp.out_color == nullptr)
			return;
		m_fp.Process();
		SetZBufferV(x, y, m_fp.fs_in.rhw);
	}

	void Rasterizer::BresenhamLine(const VS_OUT* vo0, const VS_OUT* vo1)
	{
		int x0 = vo0->pos[0];
		int y0 = vo0->pos[1];
		int x1 = vo1->pos[0];
		int y1 = vo1->pos[1];

		if (x0 < 0 || x0 > m_width)
			return;
		if (x1 < 0 || x1 > m_width)
			return;
		if (y0 < 0 || y0 > m_height)
			return;
		if (y1 < 0 || y1 > m_height)
			return;

		//DrawPixel(x0, y0, (uint32_t)(vo0->color), 5);

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
				Fragment(vo0, vo1, x, y, ratio);
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
				Fragment(vo0, vo1, x, y, ratio);
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

	void Rasterizer::Triangle(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2)
	{
		float fx1 = vo0->pos[0] + 0.5f;
		float fy1 = vo0->pos[1] + 0.5f;
		float fx2 = vo1->pos[0] + 0.5f;
		float fy2 = vo1->pos[1] + 0.5f;
		float fx3 = vo2->pos[0] + 0.5f;
		float fy3 = vo2->pos[1] + 0.5f;

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

		int y = miny;
		if (miny < 0)
		{
			int n = 0 - miny;
			Cy1 += Dx12 * n;
			Cy2 += Dx23 * n;
			Cy3 += Dx31 * n;
			y = 0;
		}
		for (; y <= maxy && y < m_height; y++)
		{
			float Cx1 = Cy1;
			float Cx2 = Cy2;
			float Cx3 = Cy3;
			int x = minx;
			if (minx < 0)
			{
				int n = 0 - minx;
				Cx1 -= Dy12 * n;
				Cx2 -= Dy23 * n;
				Cx3 -= Dy31 * n;
				x = 0;
			}
			for (; x <= maxx && x < m_width; x++)
			{
				if (Cx1 <= 0 && Cx2 <= 0 && Cx3 <= 0)
				{
					float ratio1 = ((y - y3)*(x1 - x3) - (y1 - y3)*(x - x3)) / (float)((y2 - y3)*(x1 - x3) - (y1 - y3)*(x2 - x3));
					float ratio0 = ((y - y3)*(x2 - x3) - (x - x3)*(y2 - y3)) / (float)((y1 - y3)*(x2 - x3) - (x1 - x3)*(y2 - y3));
					Fragment(vo0, vo1, vo2, x, y, ratio0, ratio1);
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

	void Rasterizer::BeginTasks()
	{
		m_taskFlag = true;
		m_mutex_async.lock();
	}

	void Rasterizer::AddTask(RasterizerTask& rt)
	{
		std::scoped_lock lock(m_mutex);
		m_tasks.push_back(rt);
	}

	void Rasterizer::EndTasks()
	{
		m_taskFlag = false;
		m_mutex_async.lock();
		m_mutex_async.unlock();
	}

	void Rasterizer::Rasterize()
	{
		while (true)
		{
			if (m_tasks_doing.size() > 0)
			{
				RasterizerTask& task = m_tasks_doing.back();
				if (task.m_vo[2] == nullptr)
				{
					BresenhamLine(task.m_vo[0], task.m_vo[1]);
				}
				else
				{
					Triangle(task.m_vo[0], task.m_vo[1], task.m_vo[2]);
				}
				m_tasks_doing.pop_back();
			}
			else if (m_tasks.size() > 0)
			{
				std::scoped_lock lock(m_mutex);
				m_tasks_doing.swap(m_tasks);
			}
			else
			{
				std::this_thread::yield();
			}

			if (m_taskFlag == false && m_tasks.size() == 0 && m_tasks_doing.size() == 0)
			{
				m_taskFlag = true;
				m_mutex_async.unlock();
			}

			//Sleep(1);
		}
	}

}