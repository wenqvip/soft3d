#pragma once

#include <windows.h>
#include <vector>
#include <map>
#include "VertexBufferObject.h"
#include "Texture.h"
#include "VertexProcessor.h"
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <dinput.h>

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

	typedef char DIKEYBOARD[256];
	typedef boost::function<void(const DIMOUSESTATE& dimouse)> MOUSE_EVENT_CB;
	typedef boost::function<void(const DIKEYBOARD& dikeyboard)> KEYBOARD_EVENT_CB;

	class Soft3dPipeline
	{
	public:
		static Soft3dPipeline* Instance()
		{
			return s_instance.get();
		}
		~Soft3dPipeline();
		void InitPipeline(HINSTANCE hInstance, HWND hwnd, uint16 width, uint16 height);
		void SetVBO(std::shared_ptr<VertexBufferObject> vbo);
		void SetUniform(uint16 index, void* uniform);
		void SetTexture(std::shared_ptr<Texture> tex);
		const Texture* CurrentTex() {
			return m_tex.get();
		}
		void Process();
		int Clear(uint32 color);

		//input
		void AddMouseEventCB(MOUSE_EVENT_CB cb) {
			m_mouseCB.push_back(cb);
		}
		void AddKeyboardEventCB(KEYBOARD_EVENT_CB cb) {
			m_keyboardCB.push_back(cb);
		}
		void LoseFocus();
		void GetFocus();


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

		LPDIRECTINPUT8 m_pDirectInput;
		LPDIRECTINPUTDEVICE8 m_pMouseDevice;
		LPDIRECTINPUTDEVICE8 m_pKeyboardDevice;

		std::vector<MOUSE_EVENT_CB> m_mouseCB;
		std::vector<KEYBOARD_EVENT_CB> m_keyboardCB;

		bool m_haveFocus;
		bool m_valid = false;
	};

	template<typename T>
	void SetUniform(uint16 index, const T& val)
	{
		T* p = new T(val);
		Soft3dPipeline::Instance()->SetUniform(index, (void*)p);
	}

}