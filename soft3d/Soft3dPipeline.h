#pragma once

#include <windows.h>
#include "VertexBufferObject.h"
#include "Texture.h"

namespace soft3d
{
	class Rasterizer;
	struct PipeLineData
	{
		vmath::vec4* pos;
		vmath::vec3* normal;
		vmath::vec2* uv;
		Color* color;
		float* rhw;

		PipeLineData()
		{
			pos = nullptr;
			normal = nullptr;
			uv = nullptr;
			color = nullptr;
			capacity = 0;
		}

		~PipeLineData()
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
		static Soft3dPipeline* Instance()
		{
			return s_instance.get();
		}
		~Soft3dPipeline();
		void InitPipeline(HWND hwnd, uint16 width, uint16 height);

		void SetVBO(std::shared_ptr<VertexBufferObject> vbo);
		VertexBufferObject* CurrentVBO();

		void SetTexture(std::shared_ptr<Texture> tex);
		const Texture* CurrentTex() {
			return m_tex.get();
		}
		void Process();

		int Clear(uint32 color);

		inline const PipeLineData* GetVSOut() const {
			return &m_vsOut;
		};

	protected:
		Soft3dPipeline();
		Soft3dPipeline(Soft3dPipeline&) {};
		static std::shared_ptr<Soft3dPipeline> s_instance;

	private:
		std::shared_ptr<VertexBufferObject> m_vbo;
		PipeLineData m_vsOut;
		std::shared_ptr<Texture> m_tex;
		std::shared_ptr<Rasterizer> m_rasterizer;

		uint16 m_width;
		uint16 m_height;
	};

}