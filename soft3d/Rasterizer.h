#pragma once
#include <thread>
#include <mutex>

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

	class Rasterizer
	{
	public:
		Rasterizer(uint16_t width, uint16_t height);
		Rasterizer(const Rasterizer& rf) = delete;
		virtual ~Rasterizer();

		int Clear(uint32_t color);
		int DrawPixel(uint16_t x, uint16_t y, uint32_t color, uint16_t size = 1);
		uint32_t* GetFBPixelPtr(uint16_t x, uint16_t y);

		void BeginTasks();
		void AddTask(RasterizerTask& rt);
		void EndTasks();

		void Fragment(const VS_OUT* vo0, const VS_OUT* vo1, uint32_t x, uint32_t y, float ratio);
		void Fragment(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, uint32_t x, uint32_t y, float ratio0, float ratio1);
		void BresenhamLine(const VS_OUT* vo0, const VS_OUT* vo1);
		void Triangle(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2);

		static const uint32_t* GetFrameBuffer() {
			return m_frameBuffer;
		}

	protected:
		void SetFrameBuffer(uint32_t index, uint32_t value);
		void SetZBufferV(uint32_t x, uint32_t y, float value);
		float GetZBufferV(uint32_t x, uint32_t y);

		void Rasterize();

	protected:
		FragmentProcessor m_fp;

		uint16_t m_width;
		uint16_t m_height;
		static uint32_t* m_frameBuffer;
		static float* m_zBuffer;
		VertexBufferObject::RENDER_MODE m_mode = VertexBufferObject::RENDER_TRIANGLE;

	private:
		std::thread m_workThread;
		std::mutex m_mutex;
		std::mutex m_mutex_async;
		std::vector<RasterizerTask> m_tasks;
		std::vector<RasterizerTask> m_tasks_doing;
		bool m_taskFlag = true;
	};

}
