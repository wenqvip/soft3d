#pragma once
#include <thread>
#include <mutex>

namespace soft3d
{
	struct FragmentData
	{
		std::mutex _mutex;
		const VS_OUT* vo0 = nullptr;//这个指针同时作为此结构体是否有数据的标识，空指针表示整个结构体没有数据
		const VS_OUT* vo1 = nullptr;
		const VS_OUT* vo2 = nullptr;

		float ratio0 = 0.0f;
		float ratio1 = 0.0f;
	};

	struct FragmentThread
	{
		FragmentProcessor* m_fragmentProcessors;
		std::thread* m_thread;
		std::vector<int> m_task;
		std::vector<int> m_task_doing;
		uint32_t m_doing_index = 0;
		std::mutex m_swap_mutex;
		std::mutex m_async_mutex;
		std::condition_variable m_condition;
		bool m_taskOver = false;
	};

	struct RasterizeData
	{
		RasterizeData(VS_OUT* vo0, VS_OUT* vo1) {
			m_vo[0] = vo0;
			m_vo[1] = vo1;
			m_vo[2] = nullptr;
		}
		RasterizeData(VS_OUT* vo0, VS_OUT* vo1, VS_OUT* vo2) {
			m_vo[0] = vo0;
			m_vo[1] = vo1;
			m_vo[2] = vo2;
		}
		~RasterizeData() = default;
		const VS_OUT* m_vo[3];
	};

	struct RasterizeThread
	{
		std::thread* m_thread;
		std::vector<RasterizeData> m_task;
		std::vector<RasterizeData> m_task_doing;
		uint32_t m_doing_index = 0;
		std::mutex m_swap_mutex;
		std::mutex m_async_mutex;
		std::condition_variable m_condition;
		bool m_taskOver = false;
	};

	class RasterizerManager
	{
	public:
		RasterizerManager(uint16_t width, uint16_t height);
		RasterizerManager(const RasterizerManager&) = delete;
		~RasterizerManager();

		int Clear(uint32_t color);
		int DrawPixel(uint16_t x, uint16_t y, uint32_t color, uint16_t size = 1);
		uint32_t* GetFBPixelPtr(uint16_t x, uint16_t y);

		void Fragment(FragmentProcessor* fp, const VS_OUT* vo0, const VS_OUT* vo1, uint32_t x, uint32_t y, float ratio);
		void Fragment(FragmentProcessor* fp, const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, uint32_t x, uint32_t y, float ratio0, float ratio1);

		void BresenhamLine(const VS_OUT* vo0, const VS_OUT* vo1);
		void Triangle(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2);

		const uint32_t* GetFrameBuffer() {
			return m_frameBuffer;
		}

		void AddRasterizeTask(VS_OUT* vo0, VS_OUT* vo1, VS_OUT* vo2);
		void AddFragTask(int id, const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, float ratio0, float ratio1);
		void BeginTask();
		void EndTask();

		void Quit();

	private:
		void FragThreadFun(int id);
		void RasterizeThreadFun(int id);

		void SetFrameBuffer(uint32_t index, uint32_t value);
		void SetZBufferV(uint32_t x, uint32_t y, float value);
		float GetZBufferV(uint32_t x, uint32_t y);

	private:
		uint16_t m_width;
		uint16_t m_height;
		uint32_t* m_frameBuffer = nullptr;
		float* m_zBuffer = nullptr;
		FragmentData* m_fragmentData = nullptr;

		int m_fragThreadCount = 1;
		int m_rasterizeThreadCount = 1;
		std::vector<FragmentThread*> m_fragThreads;
		std::vector<RasterizeThread*> m_rasterizeThreads;
		float m_thread_ratio = 0.1f;
		bool m_bQuit = true;
	};


}