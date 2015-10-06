#pragma once
#include "VertexBufferObject.h"
#include "VertexProcessor.h"
#include "FragmentProcessor.h"
#include "DirectXHelper.h"
#include <memory>

namespace soft3d
{
	struct IN_OUT
	{
		vmath::vec4* pos;
		vmath::vec3* normal;
		vmath::vec2* uv;
		uint32* color;

		IN_OUT()
		{
			pos = nullptr;
			normal = nullptr;
			uv = nullptr;
			color = nullptr;
			capacity = 0;
		}

		~IN_OUT()
		{
			if (pos != nullptr)
				delete[] pos;
			if (color != nullptr)
				delete[] color;
			if (uv != nullptr)
				delete[] uv;
			if (normal != nullptr)
				delete[] normal;
		}

		uint32 capacity;
	};

	class Soft3dPipeline
	{
	public:
		static std::shared_ptr<Soft3dPipeline> Instance()
		{
			return s_instance;
		}
		~Soft3dPipeline();

		void Process();
		void SetVBO(std::shared_ptr<VertexBufferObject> vbo);
		void Bresenhamline(int x0, int y0, int x1, int y1, const vmath::vec3* color0, const vmath::vec3* color1);

	protected:
		Soft3dPipeline();
		Soft3dPipeline(Soft3dPipeline&) {};
		static std::shared_ptr<Soft3dPipeline> s_instance;

	private:
		std::shared_ptr<VertexBufferObject> m_vbo;
		IN_OUT m_inout;
	};

}