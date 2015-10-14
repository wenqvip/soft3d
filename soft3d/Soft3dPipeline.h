#pragma once

#include <windows.h>
#include <vector>
#include <map>
#include "VertexBufferObject.h"
#include "Texture.h"
#include "VertexProcessor.h"
#include <boost/shared_array.hpp>

namespace soft3d
{
	class Rasterizer;
	struct PipeLineData
	{
		boost::shared_array<VertexProcessor> vp;

		VertexBufferObject::CULL_MODE cullMode;
		uint32 capacity;
	};

	typedef void* UniformPtr;

	class Soft3dPipeline
	{
	public:
		static Soft3dPipeline* Instance()
		{
			return s_instance.get();
		}
		~Soft3dPipeline();
		void InitPipeline(HWND hwnd, uint16 width, uint16 height);

		void SetVBO(std::shared_ptr<VertexBufferObject> vbo);

		void SetUniform(uint16 index, void* uniform);

		void SetTexture(std::shared_ptr<Texture> tex);
		const Texture* CurrentTex() {
			return m_tex.get();
		}
		void Process();

		int Clear(uint32 color);


	protected:
		Soft3dPipeline();
		Soft3dPipeline(Soft3dPipeline&) {};
		static std::shared_ptr<Soft3dPipeline> s_instance;

	private:
		std::vector<std::shared_ptr<VertexBufferObject> > m_vboVector;
		std::shared_ptr<Texture> m_tex;
		std::shared_ptr<Rasterizer> m_rasterizer;
		std::vector<std::shared_ptr<PipeLineData> > m_pipeDataVector;
		UniformPtr m_uniforms[16];

		uint16 m_width;
		uint16 m_height;
	};

	template<typename T>
	void SetUniform(uint16 index, const T& val)
	{
		T* p = new T(val);
		Soft3dPipeline::Instance()->SetUniform(index, (void*)p);
	}

}