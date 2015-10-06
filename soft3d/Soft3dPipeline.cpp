#include "stdafx.h"
#include "Soft3dPipeline.h"
#include <memory>
#include "SceneManager.h"
#include <stdlib.h>

using namespace std;
using namespace vmath;

namespace soft3d
{

	void uC2fC(uint32 color, vec3* colorf)
	{
		(*colorf)[0] = ((color & 0xff0000) >> 16) / 255.0f;
		(*colorf)[1] = ((color & 0xff00) >> 8) /255.0f;
		(*colorf)[2] = (color & 0xff) / 255.0f;
	}

	uint32 fC2uC(vec3* colorf)
	{
		for (int i = 0; i < 3; i++)
		{
			if ((*colorf)[i] > 1.0f)
				(*colorf)[i] = 1.0f;
		}
		uint32 color = 0;
		color += (uint32)(0xff * (*colorf)[0]) * 0x10000;
		color += (uint32)(0xff * (*colorf)[1]) * 0x100;
		color += 0xff * (*colorf)[2];

		return color;
	}

	std::shared_ptr<Soft3dPipeline> Soft3dPipeline::s_instance(new Soft3dPipeline());

	Soft3dPipeline::Soft3dPipeline()
	{
	}


	Soft3dPipeline::~Soft3dPipeline()
	{
	}

	void Soft3dPipeline::SetVBO(shared_ptr<VertexBufferObject> vbo)
	{
		m_vbo = vbo;
		if (m_inout.capacity < vbo->GetSize())
		{
			if(m_inout.color != nullptr)
				delete[] m_inout.color;
			if(m_inout.pos != nullptr)
				delete[] m_inout.pos;

			m_inout.color = new uint32[vbo->GetSize()];
			m_inout.pos = new vec4[vbo->GetSize()];
			m_inout.capacity = vbo->GetSize();
		}
	}
	
	void Soft3dPipeline::Bresenhamline(int x0, int y0, int x1, int y1, const vec3* color0, const vec3* color1)
	{
		int x, y, dx, dy;
		dx = x1 - x0;
		dy = y1 - y0;
		x = x0;
		y = y0;
		if (abs(dy) <= abs(dx))
		{
			int e = -abs(dx);
			for (int i = 0; i <= abs(dx); i++)
			{
				vec3 color = *color1 * (x - x0) / (float)dx;
				color += *color0 * (x1 - x) / (float)dx;
				DirectXHelper::Instance()->DrawPixel(x, y, fC2uC(&color));
				x = dx > 0 ? x + 1 : x - 1;
				e += 2 * abs(dy);
				if (e >= 0)
				{
					y = dy > 0 ? y + 1 : y - 1;
					e -= 2 * abs(dx);
				}
			}
		}
		else
		{
			int e = -abs(dy);
			for (int i = 0; i <= abs(dy); i++)
			{
				vec3 color = *color1 * (y - y0) / (float)dy;
				color += *color0 * (y1 - y) / (float)dy;
				DirectXHelper::Instance()->DrawPixel(x, y, fC2uC(&color));
				y = dy > 0 ? y + 1 : y - 1;
				e += 2 * abs(dx);
				if (e >= 0)
				{
					x = dx > 0 ? x + 1 : x - 1;
					e -= 2 * abs(dy);
				}
			}
		}
	}

	void Soft3dPipeline::Process()
	{
		SceneManager::Instance()->Update();

		if (m_vbo == nullptr)
			return;

		std::shared_ptr<VertexProcessor> vp(new VertexProcessor());
		std::shared_ptr<FragmentProcessor> fp(new FragmentProcessor());
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
			vp->out_color = &(m_inout.color[i]);
			vp->out_pos = &(m_inout.pos[i]);
			vp->Process();

			m_inout.pos[i][0] /= m_inout.pos[i][3];
			m_inout.pos[i][1] /= m_inout.pos[i][3];
			m_inout.pos[i][2] /= m_inout.pos[i][3];
			m_inout.pos[i][3] /= m_inout.pos[i][3];
		}

		for (int i = 0; i < m_vbo->GetSize(); i += 3)
		{
			for (int j = 0; j < 3; j++)
			{
				vec4 a = m_inout.pos[i] - m_inout.pos[i + 1];
				vec4 b = m_inout.pos[i + 1] - m_inout.pos[i + 2];
				vec3 c = vec3(a[0], a[1], a[2]);
				vec3 d = vec3(b[0], b[1], b[2]);
				vec3 r = cross(c, d);
				if (r[2] <= 0.0f)
					break;

				fp->color = &(m_inout.color[i + j]);
				fp->out_color = &(m_inout.color[i + j]);
				fp->Process();

				int x0 = (((const float*)m_inout.pos[i + j])[0] + 1.0f) / 2.0f * (DirectXHelper::Instance()->GetWidth());
				int y0 = (((const float*)m_inout.pos[i + j])[1] + 1.0f) / 2.0f * (DirectXHelper::Instance()->GetHeight());
				int x1 = (((const float*)m_inout.pos[i + (j+1)%3])[0] + 1.0f) / 2.0f * (DirectXHelper::Instance()->GetWidth());
				int y1 = (((const float*)m_inout.pos[i + (j+1)%3])[1] + 1.0f) / 2.0f * (DirectXHelper::Instance()->GetHeight());

				DirectXHelper::Instance()->DrawPixel(x0, y0, m_inout.color[i + j], 5);
				vec3 color0, color1;
				uC2fC(m_inout.color[i + j], &color0);
				uC2fC(m_inout.color[i + (j + 1) % 3], &color1);
				Bresenhamline(x0, y0, x1, y1, &color0, &color1);
			}
		}
	}

}