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
		m_frameBuffer = new uint32[m_width * m_height];

		soft3d::DirectXHelper::Instance()->Init(hwnd);

		SceneManager::Instance()->InitScene(m_width, m_height);
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

			m_vsOut.color = new uint32[vbo->GetSize()];
			m_vsOut.pos = new vec4[vbo->GetSize()];
			m_vsOut.capacity = vbo->GetSize();
		}
	}

	std::shared_ptr<VertexBufferObject> Soft3dPipeline::CurrentVBO()
	{
		return m_vbo;
	}

	uint32* Soft3dPipeline::GetFBPixelPtr(uint16 x, uint16 y)
	{
		y = 599 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return nullptr;

		int index = y * m_width + x;
		if (index >= (uint32)m_width * m_height)
			return nullptr;
		return &(m_frameBuffer[index]);
	}

	int Soft3dPipeline::DrawPixel(uint16 x, uint16 y, uint32 color, uint16 size)
	{
		y = 599 - y;//上下颠倒
		if (x > m_width || y > m_height)
			return -1;
		if (size > 100 || size < 1)
			size = 1;

		for (uint16 i = 0; i < size; i++)
		{
			for (uint16 j = 0; j < size; j++)
				SetFrameBuffer((y + j - size / 2) * m_width + x + i - size / 2, color);
		}
		return 0;
	}

	void Soft3dPipeline::SetFrameBuffer(uint32 index, uint32 value)
	{
		if (index >= (uint32)m_width * m_height)
			return;
		m_frameBuffer[index] = value;
	}

	int Soft3dPipeline::Clear(uint32 color)
	{
		memset(m_frameBuffer, color, m_width * m_height * sizeof(uint32));
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

			//除以w
			m_vsOut.pos[i][0] /= m_vsOut.pos[i][3];
			m_vsOut.pos[i][1] /= m_vsOut.pos[i][3];
			m_vsOut.pos[i][2] /= m_vsOut.pos[i][3];
			//m_vsOut.pos[i][3] /= m_vsOut.pos[i][3];
		}

		shared_ptr<Rasterizer> rasterizer(new Rasterizer());
		for (int i = 0; i < m_vbo->GetSize(); i += 3)
		{
			//进行背面拣选
			vec4 a = m_vsOut.pos[i] - m_vsOut.pos[i + 1];
			vec4 b = m_vsOut.pos[i + 1] - m_vsOut.pos[i + 2];
			vec3 c = vec3(a[0], a[1], a[2]);
			vec3 d = vec3(b[0], b[1], b[2]);
			vec3 r = cross(c, d);
			if (r[2] <= 0.0f)
				continue;

			//todo:进行裁剪
			//

			switch (m_vbo->m_mode)
			{
			case VertexBufferObject::RENDER_LINE:
			{
				for (int j = 0; j < 3; j++)
				{
					int x0 = (m_vsOut.pos[i + j][0] + 1.0f) / 2.0f * m_width;
					int y0 = (m_vsOut.pos[i + j][1] + 1.0f) / 2.0f * m_height;
					int x1 = (m_vsOut.pos[i + (j + 1) % 3][0] + 1.0f) / 2.0f * m_width;
					int y1 = (m_vsOut.pos[i + (j + 1) % 3][1] + 1.0f) / 2.0f * m_height;

					DrawPixel(x0, y0, m_vsOut.color[i + j], 5);

					rasterizer->BresenhamLine(x0, y0, x1, y1, i + j, i + (j + 1) % 3);
				}
				break;
			}

			case VertexBufferObject::RENDER_TRIANGLE:
			{
				int x0 = (m_vsOut.pos[i][0] + 1.0f) / 2.0f * m_width;
				int y0 = (m_vsOut.pos[i][1] + 1.0f) / 2.0f * m_height;
				int x1 = (m_vsOut.pos[i + 1][0] + 1.0f) / 2.0f * m_width;
				int y1 = (m_vsOut.pos[i + 1][1] + 1.0f) / 2.0f * m_height;
				int x2 = (m_vsOut.pos[i + 2][0] + 1.0f) / 2.0f * m_width;
				int y2 = (m_vsOut.pos[i + 2][1] + 1.0f) / 2.0f * m_height;

				rasterizer->Triangle(x0, y0, x1, y1, x2, y2, i, i + 1, i + 2);
				break;
			}

			default:
				break;
			}
		}

		DirectXHelper::Instance()->Paint(m_frameBuffer, m_width, m_height);
	}

}