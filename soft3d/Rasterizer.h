#pragma once
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include "VertexBufferObject.h"
#include <vector>

namespace soft3d
{
	struct PipeLineData;

	struct RasterizerTask
	{
		RasterizerTask(VS_OUT* vo0, VS_OUT* vo1) {
			m_vo[0] = vo0;
			m_vo[1] = vo1;
			m_vo[2] = nullptr;
		}
		RasterizerTask(VS_OUT* vo0, VS_OUT* vo1, VS_OUT* vo2) {
			m_vo[0] = vo0;
			m_vo[1] = vo1;
			m_vo[2] = vo2;
		}
		~RasterizerTask() = default;

		VS_OUT* m_vo[3];
	};

	class Rasterizer : public boost::noncopyable
	{
	public:
		Rasterizer(uint16 width, uint16 height);
		virtual ~Rasterizer();

		int Clear(uint32 color);
		int DrawPixel(uint16 x, uint16 y, uint32 color, uint16 size = 1);
		uint32* GetFBPixelPtr(uint16 x, uint16 y);

		void BeginTasks();
		void AddTask(RasterizerTask& rt);
		void EndTasks();

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

		void Rasterize();

	protected:
		FragmentProcessor m_fp;

		uint16 m_width;
		uint16 m_height;
		static uint32* m_frameBuffer;
		static float* m_zBuffer;
		VertexBufferObject::RENDER_MODE m_mode = VertexBufferObject::RENDER_TRIANGLE;

	private:
		boost::thread m_workThread;
		boost::mutex m_mutex;
		boost::mutex m_mutex_async;
		std::vector<RasterizerTask> m_tasks;
		std::vector<RasterizerTask> m_tasks_doing;
		bool m_taskFlag = true;
	};

}
