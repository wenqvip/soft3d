#pragma once

namespace soft3d
{
	struct FragmentData
	{
		boost::mutex _mutex;
		const VS_OUT* vo0 = nullptr;//这个指针同时作为此结构体是否有数据的标识，空指针表示整个结构体没有数据
		const VS_OUT* vo1 = nullptr;
		const VS_OUT* vo2 = nullptr;

		float ratio0 = 0.0f;
		float ratio1 = 0.0f;
	};

	struct FragmentThread
	{
		FragmentProcessor* m_fragmentProcessors;
		boost::thread* m_thread;
		std::vector<int> m_task;
		std::vector<int> m_task_doing;
		uint32 m_doing_index = 0;
		boost::mutex m_swap_mutex;
		boost::mutex m_async_mutex;
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
		boost::thread* m_thread;
		std::vector<RasterizeData> m_task;
		std::vector<RasterizeData> m_task_doing;
		uint32 m_doing_index = 0;
		boost::mutex m_swap_mutex;
		boost::mutex m_async_mutex;
		bool m_taskOver = false;
	};

	class RasterizerManager : public boost::noncopyable
	{
	public:
		RasterizerManager(uint16 width, uint16 height);
		~RasterizerManager();

		int Clear(uint32 color);
		int DrawPixel(uint16 x, uint16 y, uint32 color, uint16 size = 1);
		uint32* GetFBPixelPtr(uint16 x, uint16 y);

		void Fragment(FragmentProcessor* fp, const VS_OUT* vo0, const VS_OUT* vo1, uint32 x, uint32 y, float ratio);
		void Fragment(FragmentProcessor* fp, const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, uint32 x, uint32 y, float ratio0, float ratio1);

		void BresenhamLine(const VS_OUT* vo0, const VS_OUT* vo1);
		void Triangle(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2);

		const uint32* GetFrameBuffer() {
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

		void SetFrameBuffer(uint32 index, uint32 value);
		void SetZBufferV(uint32 x, uint32 y, float value);
		float GetZBufferV(uint32 x, uint32 y);

	private:
		uint16 m_width;
		uint16 m_height;
		uint32* m_frameBuffer = nullptr;
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