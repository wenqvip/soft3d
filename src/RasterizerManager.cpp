#include <functional>
#include "soft3d.h"
#include "VertexBufferObject.h"
#include <vector>
#include "FragmentProcessor.h"

#include "RasterizerManager.h"

namespace soft3d
{

	RasterizerManager::RasterizerManager(uint16_t width, uint16_t height)
	{
		m_width = width;
		m_height = height;
		if (m_frameBuffer == nullptr)
			m_frameBuffer = new uint32_t[width * height];
		if (m_zBuffer == nullptr)
			m_zBuffer = new float[width * height];
		if (m_fragmentData == nullptr)
			m_fragmentData = new FragmentData[width * height];

		m_thread_ratio = 0.7f;
		m_thread_ratio = vmath::clamp<float>(m_thread_ratio, 0.0f, 0.99f);
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		int ThreadCount = info.dwNumberOfProcessors - 3;
		if (info.dwNumberOfProcessors < 3)
		{
			m_fragThreadCount = 1;
			m_rasterizeThreadCount = 1;
		}
		else
		{
			m_fragThreadCount = vmath::ceil((float)ThreadCount * m_thread_ratio) + 1;
			m_rasterizeThreadCount = info.dwNumberOfProcessors - m_fragThreadCount - 1;
		}
		m_fragThreads.reserve(m_fragThreadCount);
		for (int i = 0; i < m_fragThreadCount; i++)
		{
			m_fragThreads.push_back(new FragmentThread());
			m_fragThreads.back()->m_thread = new std::thread(std::bind(&RasterizerManager::FragThreadFun, this, i));
			m_fragThreads.back()->m_fragmentProcessors = new FragmentProcessor();
		}
		m_rasterizeThreads.reserve(m_rasterizeThreadCount);
		for (int i = 0; i < m_rasterizeThreadCount; i++)
		{
			m_rasterizeThreads.push_back(new RasterizeThread());
			m_rasterizeThreads.back()->m_thread = new std::thread(std::bind(&RasterizerManager::RasterizeThreadFun, this, i));
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
		if (m_fragmentData)
		{
			delete[] m_fragmentData;
			m_fragmentData = nullptr;
		}
		for (int i = 0; i < m_fragThreadCount; i++)
		{
			delete m_fragThreads[i]->m_fragmentProcessors;
			m_fragThreads[i]->m_thread->join();
			delete m_fragThreads[i]->m_thread;
			delete m_fragThreads[i];
		}
		for (int i = 0; i < m_rasterizeThreadCount; i++)
		{
			m_rasterizeThreads[i]->m_thread->join();
			delete m_rasterizeThreads[i]->m_thread;
			delete m_rasterizeThreads[i];
		}
	}

	void RasterizerManager::Quit()
	{
		m_bQuit = false;
	}

	uint32_t* RasterizerManager::GetFBPixelPtr(uint16_t x, uint16_t y)
	{
		y = 599 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return nullptr;

		int index = y * m_width + x;
		if (index >= (uint32_t)m_width * m_height)
			return nullptr;
		return &(m_frameBuffer[index]);
	}

	int RasterizerManager::DrawPixel(uint16_t x, uint16_t y, uint32_t color, uint16_t size)
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

	void RasterizerManager::SetFrameBuffer(uint32_t index, uint32_t value)
	{
		if (index >= (uint32_t)m_width * m_height)
			return;
		m_frameBuffer[index] = value;
	}

	void RasterizerManager::SetZBufferV(uint32_t x, uint32_t y, float value)
	{
		y = m_height - 1 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return;
		int index = y * m_width + x;
		m_zBuffer[index] = value;
	}

	float RasterizerManager::GetZBufferV(uint32_t x, uint32_t y)
	{
		y = m_height - 1 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return 0.0f;
		int index = y * m_width + x;
		return m_zBuffer[index];
	}

	int RasterizerManager::Clear(uint32_t color)
	{
		memset(m_frameBuffer, color, m_width * m_height * sizeof(uint32_t));
		memset(m_zBuffer, 0, m_width * m_height * sizeof(float));
		return 0;
	}

	void RasterizerManager::Fragment(FragmentProcessor* fp, const VS_OUT* vo0, const VS_OUT* vo1, uint32_t x, uint32_t y, float ratio)
	{
		fp->fs_in.Interpolate(vo0, vo1, ratio, 1.0f - ratio);
		fp->fs_in.rhw += 0.001f;
		if (fp->fs_in.rhw < GetZBufferV(x, y))
			return;
		fp->tex = nullptr;// Soft3dPipeline::Instance()->CurrentTex();

		fp->out_color = GetFBPixelPtr(x, y);
		if (fp->out_color == nullptr)
			return;
		fp->fs_in.mode = VS_OUT::COLOR_MODE;
		fp->Process();
		SetZBufferV(x, y, fp->fs_in.rhw);
	}

	void RasterizerManager::Fragment(FragmentProcessor* fp, const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, uint32_t x, uint32_t y, float ratio0, float ratio1)
	{
		float ratio2 = 1.0f - ratio0 - ratio1;

		if (fp->fs_in.InterpolateRHW(vo0, vo1, vo2, ratio0, ratio1, ratio2) < GetZBufferV(x, y))
			return;

		fp->fs_in.Interpolate(vo0, vo1, vo2, ratio0, ratio1, ratio2);
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
				float z = vmath::lerp(vo0->pos[2], vo1->pos[2], ratio);
				if (-1.0f <= z && z <= 1.0f)
				{
					int id = y * m_width + x;
					AddFragTask(id, vo0, vo1, nullptr, ratio, 0.0f);
				}

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
				float z = vmath::lerp(vo0->pos[2], vo1->pos[2], ratio);
				if (-1.0f <= z && z <= 1.0f)
				{
					int id = y * m_width + x;
					AddFragTask(id, vo0, vo1, nullptr, ratio, 0.0f);
				}

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
		double fx1 = vo0->pos[0] + 0.5f;
		double fy1 = vo0->pos[1] + 0.5f;
		double fx2 = vo1->pos[0] + 0.5f;
		double fy2 = vo1->pos[1] + 0.5f;
		double fx3 = vo2->pos[0] + 0.5f;
		double fy3 = vo2->pos[1] + 0.5f;

		int x1 = fx1;
		int x2 = fx2;
		int x3 = fx3;
		int y1 = fy1;
		int y2 = fy2;
		int y3 = fy3;

		double Dx12 = fx1 - fx2;
		double Dx23 = fx2 - fx3;
		double Dx31 = fx3 - fx1;

		double Dy12 = fy1 - fy2;
		double Dy23 = fy2 - fy3;
		double Dy31 = fy3 - fy1;

		int minx = vmath::min<int>(fx1, fx2, fx3);
		int maxx = vmath::max<int>(fx1, fx2, fx3);
		int miny = vmath::min<int>(fy1, fy2, fy3);
		int maxy = vmath::max<int>(fy1, fy2, fy3);

		double C1 = Dy12 * fx1 - Dx12 * fy1;
		double C2 = Dy23 * fx2 - Dx23 * fy2;
		double C3 = Dy31 * fx3 - Dx31 * fy3;

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
					float z = vmath::lerp(vo0->pos[2], vo1->pos[2], vo2->pos[2], ratio0, ratio1);
					if (-1 <= z && z <= 1.0f)
					{
						int id = y * m_width + x;
						AddFragTask(id, vo0, vo1, vo2, ratio0, ratio1);
					}
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

	void RasterizerManager::AddRasterizeTask(VS_OUT* vo0, VS_OUT* vo1, VS_OUT* vo2)
	{
		static long index = 0;
		long id = index%m_rasterizeThreadCount;
		index++;
		std::scoped_lock lock(m_rasterizeThreads[id]->m_swap_mutex);
		m_rasterizeThreads[id]->m_task.push_back(RasterizeData(vo0, vo1, vo2));
	}

	void RasterizerManager::RasterizeThreadFun(int id)
	{
		while (m_bQuit)
		{
			RasterizeThread* rt = m_rasterizeThreads[id];
			if (rt->m_task_doing.size() > 0)
			{
				RasterizeData& data = rt->m_task_doing[rt->m_doing_index];
				if (data.m_vo[2] == nullptr)
				{
					BresenhamLine(data.m_vo[0], data.m_vo[1]);
				}
				else
				{
					Triangle(data.m_vo[0], data.m_vo[1], data.m_vo[2]);
				}

				rt->m_doing_index++;
				if (rt->m_doing_index >= rt->m_task_doing.size())
				{
					rt->m_task_doing.clear();
					rt->m_doing_index = 0;
				}
			}
			else if (rt->m_task.size() > 0)
			{
				std::scoped_lock lock(rt->m_swap_mutex);
				rt->m_task_doing.swap(rt->m_task);
			}
			else
			{
				std::this_thread::yield();
			}

			if (rt->m_taskOver == true && rt->m_task.size() == 0 && rt->m_task_doing.size() == 0)
			{
				rt->m_condition.notify_all();
			}
		}
	}

	void RasterizerManager::AddFragTask(int id, const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, float ratio0, float ratio1)
	{
		while (true)
		{
			std::scoped_lock lock(m_fragmentData[id]._mutex);
			if (m_fragmentData[id].vo0 == nullptr)
				break;
			std::this_thread::yield();
		}
		m_fragmentData[id].ratio0 = ratio0;
		m_fragmentData[id].ratio1 = ratio1;
		m_fragmentData[id].vo0 = vo0;
		m_fragmentData[id].vo1 = vo1;
		m_fragmentData[id].vo2 = vo2;

		std::scoped_lock lock(m_fragThreads[id%m_fragThreadCount]->m_swap_mutex);
		m_fragThreads[id%m_fragThreadCount]->m_task.push_back(id);
	}

	void RasterizerManager::BeginTask()
	{
		if (m_bQuit == false)
			return;
		for (int i = 0; i < m_rasterizeThreadCount; i++)
		{
			m_rasterizeThreads[i]->m_taskOver = false;
		}
		for (int i = 0; i < m_fragThreadCount; i++)
		{
			m_fragThreads[i]->m_taskOver = false;
		}
	}

	void RasterizerManager::EndTask()
	{
		if (m_bQuit == false)
			return;
		for (int i = 0; i < m_rasterizeThreadCount; i++)
		{
			m_rasterizeThreads[i]->m_taskOver = true;
			std::unique_lock<std::mutex> lock(m_rasterizeThreads[i]->m_async_mutex);
			m_rasterizeThreads[i]->m_condition.wait(lock);
			m_rasterizeThreads[i]->m_taskOver = false;
		}
		for (int i = 0; i < m_fragThreadCount; i++)
		{
			m_fragThreads[i]->m_taskOver = true;
			std::unique_lock<std::mutex> lock(m_fragThreads[i]->m_async_mutex);
			m_fragThreads[i]->m_condition.wait(lock);
			m_fragThreads[i]->m_taskOver = false;
		}
	}

	void RasterizerManager::FragThreadFun(int id)
	{
		while (m_bQuit)
		{
			FragmentThread* ft = m_fragThreads[id];
			if (ft->m_task_doing.size() > 0)
			{
				uint32_t index = ft->m_task_doing[ft->m_doing_index];
				FragmentData* data = &(m_fragmentData[index]);
				uint32_t x = index % m_width;
				uint32_t y = index / m_width;
				if (data->vo0)
				{
					if (data->vo2 == nullptr)
					{
						Fragment(ft->m_fragmentProcessors, data->vo0, data->vo1, x, y, data->ratio0);
					}
					else
					{
						//if(index % 2 == 0)
						Fragment(ft->m_fragmentProcessors, data->vo0, data->vo1, data->vo2, x, y, data->ratio0, data->ratio1);
					}
					{
						std::scoped_lock lock(data->_mutex);
						data->vo0 = nullptr;
					}
				}
				else
				{
					DrawPixel(x, y, 0xff00ff00);
				}

				ft->m_doing_index++;
				if (ft->m_doing_index >= ft->m_task_doing.size())
				{
					ft->m_task_doing.clear();
					ft->m_doing_index = 0;
				}
			}
			else if (ft->m_task.size() > 0)
			{
				std::scoped_lock lock(ft->m_swap_mutex);
				ft->m_task_doing.swap(ft->m_task);
			}
			else
			{
				//std::this_thread::yield();
			}

			if (ft->m_taskOver == true && ft->m_task.size() == 0 && ft->m_task_doing.size() == 0)
			{
				ft->m_condition.notify_all();
			}
		}
	}
}