#pragma once
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <vector>
#include "Soft3dPipeline.h"

namespace soft3d
{
	class VertexProcessorUnit : public boost::noncopyable
	{
	public:
		VertexProcessorUnit(int threadCount = 2);
		virtual ~VertexProcessorUnit();
		void SetData(std::shared_ptr<PipeLineData> pipeData, std::shared_ptr<VertexBufferObject> vbo, UniformStack uniform);
		void ThreadFun(int id);

	private:
		std::vector<boost::thread*> m_threads;
		int m_threadCount;
		std::shared_ptr<PipeLineData> m_pipeData;
		std::shared_ptr<VertexBufferObject> m_vbo;
		UniformStack m_uniform;

		boost::shared_mutex m_rwmutex;
	};

}
