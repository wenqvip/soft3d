#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include "soft3d.h"
#include "VertexBufferObject.h"
#include <vector>
#include "FragmentProcessor.h"

#include "RasterizerManager.h"

namespace soft3d
{
	uint32* RasterizerManager::m_frameBuffer = nullptr;
	float* RasterizerManager::m_zBuffer = nullptr;
	RasterizeData* RasterizerManager::m_rasterizeData = nullptr;

	RasterizerManager::RasterizerManager(uint16 width, uint16 height)
	{
		m_width = width;
		m_height = height;
		if (m_frameBuffer == nullptr)
			m_frameBuffer = new uint32[width*height*sizeof(uint32)];
		if (m_zBuffer == nullptr)
			m_zBuffer = new float[width*height*sizeof(float)];
		if (m_rasterizeData == nullptr)
			m_rasterizeData = new RasterizeData[width*height*sizeof(RasterizeData)];

		SYSTEM_INFO info;
		GetSystemInfo(&info);
		m_threadCount = info.dwNumberOfProcessors - 1;
		for (int i = 0; i < m_threadCount; i++)
		{
			m_threads.push_back(new boost::thread(boost::bind(&RasterizerManager::ThreadFun, this, i)));
			m_fragmentProcessors.push_back(new FragmentProcessor());
			m_taskStack.push_back(std::vector<uint32>());
			m_mutexs.push_back(new boost::mutex());
		}
	}

	RasterizerManager::~RasterizerManager()
	{
		if (m_frameBuffer)
		{
			delete[] m_frameBuffer;
			m_frameBuffer = nullptr;
		}
		if (m_zBuffer)
		{
			delete[] m_zBuffer;
			m_zBuffer = nullptr;
		}
		if (m_rasterizeData)
		{
			delete[] m_rasterizeData;
			m_rasterizeData = nullptr;
		}

		for (int i = 0; i < m_threadCount; i++)
		{
			delete m_threads[i];
			delete m_fragmentProcessors[i];
			delete m_mutexs[i];
		}
	}

	void RasterizerManager::ThreadFun(int id)
	{
		while (true)
		{
			if (m_taskStack_doing[id].size() > 0)
			{
				switch (m_mode)
				{
				case soft3d::VertexBufferObject::RENDER_LINE:
					break;
				case soft3d::VertexBufferObject::RENDER_TRIANGLE:
				{
					uint32 index = m_taskStack_doing[id].back();
					RasterizeData* data = &(m_rasterizeData[index]);
					uint32 x = index % m_width;
					uint32 y = index / m_width;
					Rasterize(m_fragmentProcessors[id], data->vo0, data->vo1, data->vo2, x, y, data->ratio0, data->ratio1);
				}
				break;
				default:
					break;
				}
			}
			else if (m_taskStack[id].size() > 0)
			{
				boost::mutex::scoped_lock lock(*m_mutexs[id]);
				m_taskStack_doing[id].swap(m_taskStack[id]);
			}


		}
	}

	uint32* RasterizerManager::GetFBPixelPtr(uint16 x, uint16 y)
	{
		y = 599 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return nullptr;

		int index = y * m_width + x;
		if (index >= (uint32)m_width * m_height)
			return nullptr;
		return &(m_frameBuffer[index]);
	}

	int RasterizerManager::DrawPixel(uint16 x, uint16 y, uint32 color, uint16 size)
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

	void RasterizerManager::SetFrameBuffer(uint32 index, uint32 value)
	{
		if (index >= (uint32)m_width * m_height)
			return;
		m_frameBuffer[index] = value;
	}

	void RasterizerManager::SetZBufferV(uint32 x, uint32 y, float value)
	{
		y = m_height - 1 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return;
		int index = y * m_width + x;
		m_zBuffer[index] = value;
	}

	float RasterizerManager::GetZBufferV(uint32 x, uint32 y)
	{
		y = m_height - 1 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return 0.0f;
		int index = y * m_width + x;
		return m_zBuffer[index];
	}

	int RasterizerManager::Clear(uint32 color)
	{
		memset(m_frameBuffer, color, m_width * m_height * sizeof(uint32));
		memset(m_zBuffer, 0, m_width* m_height* sizeof(float));
		return 0;
	}

	void RasterizerManager::Rasterize(FragmentProcessor* fp, const VS_OUT* vo0, const VS_OUT* vo1, uint32 x, uint32 y, float ratio)
	{
		fp->fs_in.Interpolate(vo0, vo1, ratio, 1.0f - ratio);
		if (fp->fs_in.rhw < GetZBufferV(x, y))
			return;
		fp->tex = Soft3dPipeline::Instance()->CurrentTex();

		fp->out_color = GetFBPixelPtr(x, y);
		if (fp->out_color == nullptr)
			return;
		fp->Process();
		SetZBufferV(x, y, fp->fs_in.rhw);
	}

	void RasterizerManager::Rasterize(FragmentProcessor* fp, const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, uint32 x, uint32 y, float ratio0, float ratio1)
	{
		float ratio2 = 1.0f - ratio0 - ratio1;
		fp->fs_in.Interpolate(vo0, vo1, vo2, ratio0, ratio1, ratio2);
		if (fp->fs_in.rhw < GetZBufferV(x, y))
			return;

		fp->tex = Soft3dPipeline::Instance()->CurrentTex();

		fp->out_color = GetFBPixelPtr(x, y);
		if (fp->out_color == nullptr)
			return;
		fp->Process();
		SetZBufferV(x, y, fp->fs_in.rhw);
	}

	void RasterizerManager::BresenhamLine(const VS_OUT* vo0, const VS_OUT* vo1)
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

		DrawPixel(x0, y0, (uint32)(vo0->color), 5);

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
				int id = x * m_width + y;
				AddTask(id, vo0, vo1, nullptr, x, y, ratio, 0.0f);
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
				int id = x * m_width + y;
				AddTask(id, vo0, vo1, nullptr, x, y, ratio, 0.0f);
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

	void RasterizerManager::Triangle(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2)
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
					int id = x * m_width + y;
					AddTask(id, vo0, vo1, vo2, x, y, ratio0, ratio1);
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

	void RasterizerManager::AddTask(int id, const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, uint32 x, uint32 y, float ratio0, float ratio1)
	{
		m_rasterizeData[id].ratio0 = ratio0;
		m_rasterizeData[id].ratio1 = ratio1;
		m_rasterizeData[id].vo0 = vo0;
		m_rasterizeData[id].vo1 = vo1;
		m_rasterizeData[id].vo2 = vo2;

		boost::mutex::scoped_lock lock(*m_mutexs[id%m_threadCount]);
		m_taskStack[id%m_threadCount].push_back(id);
	}
}