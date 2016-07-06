#include "soft3d.h"
#include "VertexProcessorUnit.h"
#include <boost/bind.hpp>

namespace soft3d
{
	VertexProcessorUnit::VertexProcessorUnit(int threadCount) :
		m_threadCount(threadCount)
	{
		for (int i = 0; i < m_threadCount; i++)
		{
			m_threads.push_back(new boost::thread(boost::bind(&VertexProcessorUnit::ThreadFun, this, i)));
		}
	}


	VertexProcessorUnit::~VertexProcessorUnit()
	{
		for (int i = 0; i < m_threadCount; i++)
		{
			delete m_threads[i];
		}
	}

	void VertexProcessorUnit::SetData(std::shared_ptr<PipeLineData> pipeData, std::shared_ptr<VertexBufferObject> vbo, UniformStack uniform)
	{
		boost::unique_lock<boost::shared_mutex> wlock(m_rwmutex);
		m_pipeData = pipeData;
		m_vbo = vbo;
		m_uniform = uniform;
	}

	void VertexProcessorUnit::ThreadFun(int id)
	{
		while (true)
		{
			if (m_pipeData == nullptr || m_vbo == nullptr || m_uniform == nullptr)
			{
				Sleep(1);
				continue;
			}
			boost::shared_lock<boost::shared_mutex> rlock(m_rwmutex);
			for (int i = 0; i < m_pipeData->capacity; i++)
			{
				VertexProcessor& cur_vp = m_pipeData->vp[i];
				const uint32* colorptr = nullptr;
				if (m_vbo->useIndex())
				{
					colorptr = m_vbo->GetColor(m_vbo->GetIndex(i));
					cur_vp.pos = m_vbo->GetPos(m_vbo->GetIndex(i));
				}
				else
				{
					colorptr = m_vbo->GetColor(i);
					cur_vp.pos = m_vbo->GetPos(i);
				}
				if (colorptr != nullptr)
					cur_vp.color = colorptr;
				else
					cur_vp.color = (uint32*)this;//随机颜色
				cur_vp.normal = m_vbo->GetNormal(i);

				if (m_vbo->hasUV())
					cur_vp.vs_out.uv = *(m_vbo->GetUV(i));

				cur_vp.uniforms = m_uniform;
				cur_vp.Process();//这一步进行视图变换和投影变换

								 //除以w
				float rhw = 1.0f / cur_vp.vs_out.pos[3];
				cur_vp.vs_out.pos[0] *= rhw;
				cur_vp.vs_out.pos[1] *= rhw;
				cur_vp.vs_out.pos[2] *= rhw;
				cur_vp.vs_out.pos[3] = 1.0f;
				cur_vp.vs_out.rhw = rhw;

				cur_vp.vs_out.pos[0] = (cur_vp.vs_out.pos[0] + 1.0f) * 0.5f * Soft3dPipeline::Instance()->GetWidth();
				cur_vp.vs_out.pos[1] = (cur_vp.vs_out.pos[1] + 1.0f) * 0.5f * Soft3dPipeline::Instance()->GetHeight();

				cur_vp.vs_out.uv *= rhw;//uv在这里除以w，以后乘回来，为了能正确计算纹理uv
			}
		}
	}

}