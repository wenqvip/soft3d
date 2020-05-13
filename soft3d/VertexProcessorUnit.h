#pragma once
#include <vector>
#include <thread>
#include <shared_mutex>
#include "Soft3dPipeline.h"

namespace soft3d
{
	class VertexProcessorUnit
	{
	public:
		VertexProcessorUnit(int threadCount = 2);
		VertexProcessorUnit(const VertexProcessorUnit& rf) = delete;
		virtual ~VertexProcessorUnit();
		void SetData(std::shared_ptr<PipeLineData> pipeData, std::shared_ptr<VertexBufferObject> vbo, UniformStack uniform);
		void ThreadFun(int id);

	private:
		std::vector<std::thread*> m_threads;
		int m_threadCount;
		std::shared_ptr<PipeLineData> m_pipeData;
		std::shared_ptr<VertexBufferObject> m_vbo;
		UniformStack m_uniform;

		std::shared_mutex m_rwmutex;
	};

}
