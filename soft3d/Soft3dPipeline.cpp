#include <stdlib.h>
#include "soft3d.h"
#include "SceneManager.h"
#include "DirectXHelper.h"
#include "VertexProcessor.h"
#include "FragmentProcessor.h"
#include "Rasterizer.h"

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
	}

	void Soft3dPipeline::InitPipeline(HWND hwnd, uint16 width, uint16 height)
	{
		m_width = width;
		m_height = height;
		m_rasterizer = shared_ptr<Rasterizer>(new Rasterizer(width, height));
		soft3d::DirectXHelper::Instance()->Init(hwnd);
		SceneManager::Instance()->InitScene(width, height);
	}

	void Soft3dPipeline::SetVBO(shared_ptr<VertexBufferObject> vbo)
	{
		m_vbo = vbo;
		if (m_vsOut.capacity < vbo->GetSize())
		{
			if(m_vsOut.color != nullptr)
				delete[] m_vsOut.color;
			if(m_vsOut.pos != nullptr)
				delete[] m_vsOut.pos;

			m_vsOut.color = new Color[vbo->GetSize()];
			m_vsOut.pos = new vec4[vbo->GetSize()];
			m_vsOut.uv = new vec2[vbo->GetSize()];
			m_vsOut.capacity = vbo->GetSize();
		}
	}

	VertexBufferObject* Soft3dPipeline::CurrentVBO()
	{
		return m_vbo.get();
	}

	void Soft3dPipeline::SetTexture(shared_ptr<Texture> tex)
	{
		m_tex = tex;
	}

	int Soft3dPipeline::Clear(uint32 color)
	{
		m_rasterizer->Clear(color);
		return 0;
	}

	void Soft3dPipeline::Process()
	{
		SceneManager::Instance()->Update();

		if (m_vbo == nullptr)
			return;

		std::shared_ptr<VertexProcessor> vp(new VertexProcessor());
		for (int i = 0; i < m_vbo->GetSize(); i++)
		{
			if (m_vbo->useIndex())
			{
				vp->color = m_vbo->GetColor(m_vbo->GetIndex(i));
				vp->pos = m_vbo->GetPos(m_vbo->GetIndex(i));
			}
			else
			{
				vp->color = m_vbo->GetColor(i);
				vp->pos = m_vbo->GetPos(i);
			}
			vp->mv_matrix = &(m_vbo->mv_matrix);
			vp->proj_matrix = &(m_vbo->proj_matrix);
			vp->out_color = &(m_vsOut.color[i]);
			vp->out_pos = &(m_vsOut.pos[i]);
			vp->Process();//这一步进行视图变换和投影变换

			if(m_vbo->hasUV())
				m_vsOut.uv[i] = *(m_vbo->GetUV(i));

			//除以w
			m_vsOut.pos[i][0] /= m_vsOut.pos[i][3];
			m_vsOut.pos[i][1] /= m_vsOut.pos[i][3];
			m_vsOut.pos[i][2] /= m_vsOut.pos[i][3];
			//m_vsOut.pos[i][3] /= m_vsOut.pos[i][3];
		}

		for (int i = 0; i < m_vbo->GetSize(); i += 3)
		{
			if (m_vbo->m_cullMode != VertexBufferObject::CULL_NONE)
			{
				//进行背面拣选
				vec4 a = m_vsOut.pos[i] - m_vsOut.pos[i + 1];
				vec4 b = m_vsOut.pos[i + 1] - m_vsOut.pos[i + 2];
				vec3 c = vec3(a[0], a[1], a[2]);
				vec3 d = vec3(b[0], b[1], b[2]);
				vec3 r = cross(c, d);
				if (m_vbo->m_cullMode == VertexBufferObject::CULL_CCW)
				{
					if (r[2] <= 0.0f)
						continue;
				}
				else
				{
					if (r[2] >= 0.0f)
						continue;
				}
			}

			//todo:进行裁剪
			//

			switch (m_vbo->m_mode)
			{
			case VertexBufferObject::RENDER_LINE:
			{
				for (int j = 0; j < 3; j++)
				{
					m_rasterizer->BresenhamLine(&m_vsOut, i + j, i + (j + 1) % 3);
				}
				break;
			}

			case VertexBufferObject::RENDER_TRIANGLE:
			{

				m_rasterizer->Triangle(&m_vsOut, i, i + 1, i + 2);
				break;
			}

			default:
				break;
			}
		}

		DirectXHelper::Instance()->Paint(m_rasterizer->GetFrameBuffer(), m_width, m_height);
	}

}