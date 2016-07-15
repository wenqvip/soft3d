#include <stdlib.h>
#include "soft3d.h"
#include "SceneManager.h"
#include "DirectXHelper.h"
#include "VertexProcessor.h"
#include "FragmentProcessor.h"
#include "Rasterizer.h"
#include "RasterizerManager.h"
#include <boost/foreach.hpp>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "DXGuid.lib")

using namespace std;
using namespace vmath;

namespace soft3d
{

	/*Color operator*(const Color& lf, float ratio)
	{
		Color cc;
		cc.B = lf.B * ratio;
		cc.G = lf.G * ratio;
		cc.R = lf.R * ratio;
		cc.A = lf.A * ratio;
		return cc;
	}*/

	/*Color operator+(const Color& lf, const Color& rf)
	{
		Color cc;
		cc.B = lf.B + rf.B;
		cc.G = lf.G + rf.G;
		cc.R = lf.R + rf.R;
		cc.A = lf.A + rf.A;
		return cc;
	}*/

	std::shared_ptr<Soft3dPipeline> Soft3dPipeline::s_instance(new Soft3dPipeline());

	Soft3dPipeline::Soft3dPipeline()
	{
	}


	Soft3dPipeline::~Soft3dPipeline()
	{
		BOOST_FOREACH(UniformPtr* us, m_UniformVector)
		{
			for (int i = 0; i < 16; i++)
			{
				if (us[i] != nullptr)
					delete us[i];
			}
		}
	}

	void Soft3dPipeline::InitPipeline(HINSTANCE hInstance, HWND hwnd, uint16 width, uint16 height)
	{
		m_threadMode = THREAD_MULTI_FRAGMENT;
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		THREAD_COUNT = info.dwNumberOfProcessors - 1;
		m_width = width;
		m_height = height;
		if (m_threadMode == THREAD_MULTI_RASTERIZER)
		{
			for (int i = 0; i < THREAD_COUNT; i++)
				m_rasterizers.push_back(shared_ptr<Rasterizer>(new Rasterizer(width, height)));
		}
		else if (m_threadMode == THREAD_MULTI_FRAGMENT)
		{
			m_rasterizerManager = shared_ptr<RasterizerManager>(new RasterizerManager(width, height));
		}
		else
		{
			m_rasterizer = shared_ptr<Rasterizer>(new Rasterizer(width, height));
		}
		soft3d::DirectXHelper::Instance()->Init(hwnd);
		SceneManager::Instance()->InitScene(width, height);

		DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);

		m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, NULL);
		m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
		m_pMouseDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		m_pMouseDevice->Acquire();

		m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, NULL);
		m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
		m_pKeyboardDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		m_pKeyboardDevice->Acquire();

		m_valid = true;
	}

	int Soft3dPipeline::SetVBO(shared_ptr<VertexBufferObject> vbo)
	{
		shared_ptr<PipeLineData> pd(new PipeLineData());
		pd->vp = boost::shared_array<VertexProcessor>(new VertexProcessor[vbo->GetSize()]);
		pd->cullMode = vbo->m_cullMode;
		pd->renderMode = vbo->m_mode;
		pd->capacity = vbo->GetSize();
		UniformStack stack = new UniformPtr[16]{ nullptr };

		m_pipeDataVector.push_back(pd);
		m_vboVector.push_back(vbo);
		m_UniformVector.push_back(stack);
		m_curVBO = m_vboVector.size() - 1;
		return m_curVBO;
	}

	void Soft3dPipeline::SelectVBO(uint32 vboIndex)
	{
		m_curVBO = vboIndex;
	}

	void Soft3dPipeline::SetUniform(uint16 index, void* uniform)
	{
		if (index < VertexBufferObject::MAX_UNIFORM_COUNT)
		{
			if (m_UniformVector[m_curVBO][index] != nullptr)
			{
				delete m_UniformVector[m_curVBO][index];
				m_UniformVector[m_curVBO][index] = nullptr;
			}
			m_UniformVector[m_curVBO][index] = uniform;
		}
	}

	void Soft3dPipeline::SetTexture(shared_ptr<Texture> tex)
	{
		m_tex = tex;
	}

	int Soft3dPipeline::Clear(uint32 color)
	{
		if (m_threadMode == THREAD_MULTI_RASTERIZER)
		{
			for (int i = 0; i < THREAD_COUNT; i++)
				m_rasterizers[i]->Clear(color);
		}
		else if (m_threadMode == THREAD_MULTI_FRAGMENT)
		{
			m_rasterizerManager->Clear(color);
		}
		else
		{
			m_rasterizer->Clear(color);
		}
		return 0;
	}

	void Soft3dPipeline::Process()
	{
		if (m_haveFocus == false)
		{
			Sleep(50);
			return;
		}
		DirectXHelper::Instance()->Profile(GetTickCount(), L"");
		DIMOUSESTATE dimouse;
		m_pMouseDevice->GetDeviceState(sizeof(dimouse), (LPVOID)&dimouse);
		BOOST_FOREACH(MOUSE_EVENT_CB cb, m_mouseCB)
		{
			cb(dimouse);
		}
		DIKEYBOARD dkeyboard;
		m_pKeyboardDevice->GetDeviceState(sizeof(dkeyboard), (LPVOID)dkeyboard);
		BOOST_FOREACH(KEYBOARD_EVENT_CB cb, m_keyboardCB)
		{
			cb(dkeyboard);
		}
		DirectXHelper::Instance()->Profile(GetTickCount(), L"Input");

		SceneManager::Instance()->Update();

		if (m_threadMode == THREAD_MULTI_RASTERIZER)
		{
			for (int i = 0; i < THREAD_COUNT; i++)
				m_rasterizers[i]->BeginTasks();
		}
		else if (m_threadMode == THREAD_MULTI_FRAGMENT)
		{
			m_rasterizerManager->BeginTask();
		}

		DirectXHelper::Instance()->Profile(GetTickCount(), L"Scene");
		for (int idx = 0; idx < m_pipeDataVector.size(); idx++)
		{
			shared_ptr<PipeLineData>& pipeData = m_pipeDataVector[idx];
			VertexBufferObject* vbo = m_vboVector[idx].get();
			for (int i = 0; i < pipeData->capacity; i++)
			{
				VertexProcessor& cur_vp = pipeData->vp[i];
				const uint32* colorptr = nullptr;
				if (vbo->useIndex())
				{
					colorptr = vbo->GetColor(vbo->GetIndex(i));
					cur_vp.pos = vbo->GetPos(vbo->GetIndex(i));
				}
				else
				{
					colorptr = vbo->GetColor(i);
					cur_vp.pos = vbo->GetPos(i);
				}
				if (colorptr != nullptr)
					cur_vp.color = colorptr;
				else
					cur_vp.color = (uint32*)this;//随机颜色
				cur_vp.normal = vbo->GetNormal(i);

				if (vbo->hasUV())
					cur_vp.vs_out.uv = *(vbo->GetUV(i));

				//cur_vp.vs_out.uv[0] = 1.0 - cur_vp.vs_out.uv[0];
				cur_vp.vs_out.uv[1] = 1.0 - cur_vp.vs_out.uv[1];//资源里的uv是从左下角开始算，而管线的uv从左上开始算，所以这里上下翻转

				cur_vp.uniforms = m_UniformVector[idx];
				cur_vp.Process();//这一步进行视图变换和投影变换

				//除以w
				float rhw = 1.0f / cur_vp.vs_out.pos[3];
				cur_vp.vs_out.pos[0] *= rhw;
				cur_vp.vs_out.pos[1] *= rhw;
				cur_vp.vs_out.pos[2] *= rhw;
				cur_vp.vs_out.pos[3] = 1.0f;
				cur_vp.vs_out.rhw = rhw;

				cur_vp.vs_out.pos[0] = (cur_vp.vs_out.pos[0] + 1.0f) * 0.5f * m_width;
				cur_vp.vs_out.pos[1] = (cur_vp.vs_out.pos[1] + 1.0f) * 0.5f * m_height;

				cur_vp.vs_out.uv *= rhw;//uv在这里除以w，以后乘回来，为了能正确计算纹理uv
			}
			DirectXHelper::Instance()->Profile(GetTickCount(), L"VP");
			for (int i = 0; i < pipeData->capacity; i += 3)
			{
				VertexProcessor& vp1 = pipeData->vp[i];
				VertexProcessor& vp2 = pipeData->vp[i + 1];
				VertexProcessor& vp3 = pipeData->vp[i + 2];

				VertexBufferObject::CULL_MODE cull_mode = VertexBufferObject::CULL_NONE;
				//进行背面拣选
				vec4 a = vp1.vs_out.pos - vp2.vs_out.pos;
				vec4 b = vp2.vs_out.pos - vp3.vs_out.pos;
				vec3 c = vec3(a[0], a[1], a[2]);
				vec3 d = vec3(b[0], b[1], b[2]);
				vec3 r = cross(c, d);
				if (r[2] < 0.0f)
					cull_mode = VertexBufferObject::CULL_CW;
				else if (r[2] > 0.0f)
					cull_mode = VertexBufferObject::CULL_CCW;

				if (pipeData->cullMode != VertexBufferObject::CULL_NONE && pipeData->cullMode != cull_mode)
					continue;

				//进行裁剪
				if (vp1.vs_out.pos[2] < 0.0f
					|| vp2.vs_out.pos[2] < 0.0f
					|| vp3.vs_out.pos[2] < 0.0f)
					continue;
				//if (vp1.vs_out.pos[0] > m_width * 1.0f || vp1.vs_out.pos[1] > m_height * 1.0f
				//	|| vp1.vs_out.pos[0] < 0 || vp1.vs_out.pos[1] < 0)
				//	continue;
				//if (vp2.vs_out.pos[0] > m_width * 1.0f || vp2.vs_out.pos[1] > m_height * 1.0f
				//	|| vp2.vs_out.pos[0] < 0 || vp2.vs_out.pos[1] < 0)
				//	continue;
				//if (vp3.vs_out.pos[0] > m_width * 1.0f || vp3.vs_out.pos[1] > m_height * 1.0f
				//	|| vp3.vs_out.pos[0] < 0 || vp3.vs_out.pos[1] < 0)
				//	continue;

				//make triangle always ccw sorting
				uint32 index[3] = { i, i + 1, i + 2 };
				if (cull_mode == VertexBufferObject::CULL_CW)
				{
					index[0] = i + 2;
					index[2] = i;
				}

				switch (pipeData->renderMode)
				{
				case VertexBufferObject::RENDER_LINE:
				{
					if (m_threadMode == THREAD_MULTI_RASTERIZER)
					{
						m_rasterizers[(i/3)%THREAD_COUNT]->AddTask(RasterizerTask(&(pipeData->vp[index[0]].vs_out), &(pipeData->vp[index[1]].vs_out)));
						m_rasterizers[(i/3)%THREAD_COUNT]->AddTask(RasterizerTask(&(pipeData->vp[index[1]].vs_out), &(pipeData->vp[index[2]].vs_out)));
						m_rasterizers[(i/3)%THREAD_COUNT]->AddTask(RasterizerTask(&(pipeData->vp[index[2]].vs_out), &(pipeData->vp[index[0]].vs_out)));
					}
					else if (m_threadMode == THREAD_MULTI_FRAGMENT)
					{
						m_rasterizerManager->AddRasterizeTask(&(pipeData->vp[index[0]].vs_out), &(pipeData->vp[index[1]].vs_out), nullptr);
						m_rasterizerManager->AddRasterizeTask(&(pipeData->vp[index[1]].vs_out), &(pipeData->vp[index[2]].vs_out), nullptr);
						m_rasterizerManager->AddRasterizeTask(&(pipeData->vp[index[2]].vs_out), &(pipeData->vp[index[0]].vs_out), nullptr);
					}
					else
					{
						m_rasterizer->BresenhamLine(&(pipeData->vp[index[0]].vs_out), &(pipeData->vp[index[1]].vs_out));
						m_rasterizer->BresenhamLine(&(pipeData->vp[index[1]].vs_out), &(pipeData->vp[index[2]].vs_out));
						m_rasterizer->BresenhamLine(&(pipeData->vp[index[2]].vs_out), &(pipeData->vp[index[0]].vs_out));
					}

					break;
				}

				case VertexBufferObject::RENDER_TRIANGLE:
				{
					if (m_threadMode == THREAD_MULTI_RASTERIZER)
					{
						m_rasterizers[(i/3)%THREAD_COUNT]->AddTask(RasterizerTask(&(pipeData->vp[index[0]].vs_out), &(pipeData->vp[index[1]].vs_out), &(pipeData->vp[index[2]].vs_out)));
					}
					else if (m_threadMode == THREAD_MULTI_FRAGMENT)
					{
						m_rasterizerManager->AddRasterizeTask(&(pipeData->vp[index[0]].vs_out), &(pipeData->vp[index[1]].vs_out), &(pipeData->vp[index[2]].vs_out));
						//m_rasterizerManager->Triangle(&(pipeData->vp[index[0]].vs_out), &(pipeData->vp[index[1]].vs_out), &(pipeData->vp[index[2]].vs_out));
					}
					else
					{
						m_rasterizer->Triangle(&(pipeData->vp[index[0]].vs_out), &(pipeData->vp[index[1]].vs_out), &(pipeData->vp[index[2]].vs_out));
					}
					break;
				}

				default:
					break;
				}
			}
			DirectXHelper::Instance()->Profile(GetTickCount(), L"FP_push");
		}
		if (m_threadMode == THREAD_MULTI_RASTERIZER)
		{
			for (int i = 0; i < THREAD_COUNT; i++)
			{
				m_rasterizers[i]->EndTasks();
			}
		}
		else if (m_threadMode == THREAD_MULTI_FRAGMENT)
		{
			m_rasterizerManager->EndTask();
		}
		DirectXHelper::Instance()->Profile(GetTickCount(), L"FP_wait");

		if (m_threadMode == THREAD_MULTI_FRAGMENT)
			DirectXHelper::Instance()->Paint(m_rasterizerManager->GetFrameBuffer(), m_width, m_height);
		else
			DirectXHelper::Instance()->Paint(Rasterizer::GetFrameBuffer(), m_width, m_height);
		DirectXHelper::Instance()->Profile(GetTickCount(), L"BLT");
	}

	void Soft3dPipeline::LoseFocus()
	{
		m_haveFocus = false;
	}

	void Soft3dPipeline::GetFocus()
	{
		if (m_valid)
		{
			m_haveFocus = true;
			m_pMouseDevice->Acquire();
			m_pKeyboardDevice->Acquire();
		}
	}

}