#pragma once

#include <windows.h>
#include "VertexBufferObject.h"

namespace soft3d
{
	struct VS_OUT
	{
		vmath::vec4* pos;
		vmath::vec3* normal;
		vmath::vec2* uv;
		uint32* color;

		VS_OUT()
		{
			pos = nullptr;
			normal = nullptr;
			uv = nullptr;
			color = nullptr;
			capacity = 0;
		}

		~VS_OUT()
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
		std::shared_ptr<VertexBufferObject> CurrentVBO();
		int Clear(uint32 color);
		void Process();

		int DrawPixel(uint16 x, uint16 y, uint32 color, uint16 size = 1);
		uint32* GetFBPixelPtr(uint16 x, uint16 y);
		inline const VS_OUT* GetVSOut() const {
			return &m_vsOut;
		};

	protected:
		Soft3dPipeline();
		Soft3dPipeline(Soft3dPipeline&) {};
		static std::shared_ptr<Soft3dPipeline> s_instance;

		void SetFrameBuffer(uint32 index, uint32 value);

	private:
		std::shared_ptr<VertexBufferObject> m_vbo;
		VS_OUT m_vsOut;

		uint16 m_width;
		uint16 m_height;
		uint32* m_frameBuffer;
	};

}