#pragma once

namespace soft3d
{
	struct RasterizeData
	{
		const VS_OUT* vo0;
		const VS_OUT* vo1;
		const VS_OUT* vo2;

		float ratio0;
		float ratio1;
	};

	class RasterizerManager : public boost::noncopyable
	{
	public:
		RasterizerManager(uint16 width, uint16 height);
		virtual ~RasterizerManager();

		int Clear(uint32 color);
		int DrawPixel(uint16 x, uint16 y, uint32 color, uint16 size = 1);
		uint32* GetFBPixelPtr(uint16 x, uint16 y);

		void Rasterize(FragmentProcessor* fp, const VS_OUT* vo0, const VS_OUT* vo1, uint32 x, uint32 y, float ratio);
		void Rasterize(FragmentProcessor* fp, const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, uint32 x, uint32 y, float ratio0, float ratio1);

		void BresenhamLine(const VS_OUT* vo0, const VS_OUT* vo1);
		void Triangle(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2);

		const uint32* GetFrameBuffer() {
			return m_frameBuffer;
		}

		void AddTask(int id, const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, uint32 x, uint32 y, float ratio0, float ratio1);

	protected:
		void ThreadFun(int id);

		void SetFrameBuffer(uint32 index, uint32 value);
		void SetZBufferV(uint32 x, uint32 y, float value);
		float GetZBufferV(uint32 x, uint32 y);

	protected:
		uint16 m_width;
		uint16 m_height;
		static uint32* m_frameBuffer;
		static float* m_zBuffer;
		static RasterizeData* m_rasterizeData;
		VertexBufferObject::RENDER_MODE m_mode = VertexBufferObject::RENDER_TRIANGLE;
		std::vector<FragmentProcessor*> m_fragmentProcessors;

	private:
		int m_threadCount = 1;
		std::vector<boost::thread*> m_threads;
		std::vector<std::vector<uint32>> m_taskStack;
		std::vector<std::vector<uint32>> m_taskStack_doing;
		std::vector<boost::mutex*> m_mutexs;
	};


}