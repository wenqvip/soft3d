#include "soft3d.h"
#include "SceneManagerPlane.h"
#include "TextureLoader.h"
#include "FbxLoader.h"
#include <functional>

using namespace std;
using namespace vmath;

namespace soft3d
{


	void SceneManagerPlane::InitScene(uint16_t width, uint16_t height)
	{
		m_width = width;
		m_height = height;

		FbxLoader fbxLoader;
		fbxLoader.LoadFbx("plane2x2.fbx");

		shared_ptr<VertexBufferObject> vbo(new VertexBufferObject());
		vbo->CopyVertexBuffer(fbxLoader.GetVertexBuffer(), fbxLoader.GetVertexCount() * 4);
		vbo->CopyIndexBuffer(fbxLoader.GetIndexBuffer(), fbxLoader.GetIndexCount());
		vbo->CopyNormalBuffer(fbxLoader.GetNormalBuffer(), fbxLoader.GetNormalCount() * 3);
		vbo->CopyUVBuffer(fbxLoader.GetUVBuffer(), fbxLoader.GetUVCount() * 2);

		vbo->m_mode = VertexBufferObject::RENDER_TRIANGLE;
		vbo->m_cullMode = VertexBufferObject::CULL_NONE;
		m_vbo1 = Soft3dPipeline::Instance()->SetVBO(vbo);
		//vbo->m_mode = VertexBufferObject::RENDER_LINE;
		//m_vbo2 = Soft3dPipeline::Instance()->SetVBO(vbo);

		shared_ptr<Texture> tex(new Texture());

		//TextureLoader::Instance().LoadTexture(L"cathead_small.png");
		//tex->CopyFromBuffer(TextureLoader::Instance().GetData(), TextureLoader::Instance().GetWidth(), TextureLoader::Instance().GetHeight());

		uint32_t tex_data[] = {
			0x0FFFFF, 0x000FF0,
			0x000FF0, 0x0FFFFF,
		};
		tex->CopyFromBuffer(tex_data, 2, 2);

		tex->filter_mode = Texture::NEAREST;
		Soft3dPipeline::Instance()->SetTexture(tex);

		Soft3dPipeline::Instance()->AddKeyboardEventCB(std::bind(&SceneManagerPlane::KeyboardEventCB, this, std::placeholders::_1));
	}

	void SceneManagerPlane::Update()
	{
		float aspect = (float)m_width / (float)m_height;
		mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
		mat4 view_matrix = lookat(vec3(0.0f, 0.0f, 3.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f));
		float factor = 180;// ((int)GetTickCount() / 50 % 180) - 90;
		mat4 mv_matrix = view_matrix
			* translate(m_x_offset, m_y_offset, m_z_offset)
			* scale(1.0f)
			* rotate(m_x_angle, vec3(1.0f, 0.0f, 0.0f))
			* rotate(m_y_angle, vec3(0.0f, 1.0f, 0.0f))
			* rotate(m_z_angle, vec3(0.0f, 0.0f, 1.0f));

		Soft3dPipeline::Instance()->SelectVBO(m_vbo1);
		SetUniform(UNIFORM_MV_MATRIX, mv_matrix);
		SetUniform(UNIFORM_PROJ_MATRIX, proj_matrix);
		//SetUniform(UNIFORM_LIGHT_POS, vec3(0.0f, 0.0f, -100.0f));
		//Soft3dPipeline::Instance()->SelectVBO(m_vbo2);
		//SetUniform(UNIFORM_MV_MATRIX, mv_matrix);
		//SetUniform(UNIFORM_PROJ_MATRIX, proj_matrix);
		//SetUniform(UNIFORM_LIGHT_POS, vec3(0.0f, 0.0f, -100.0f));

		Soft3dPipeline::Instance()->Clear(0xffffff);
	}

	void SceneManagerPlane::KeyboardEventCB(const DIKEYBOARD dikeyboard)
	{
		if (dikeyboard[DIK_A] & 0x80)
		{
			m_z_angle += 1.0f;
		}
		else if (dikeyboard[DIK_D] & 0x80)
		{
			m_z_angle -= 1.0f;
		}

		if (dikeyboard[DIK_W] & 0x80)
		{
			m_x_angle += 1.0f;
		}
		else if (dikeyboard[DIK_S] & 0x80)
		{
			m_x_angle -= 1.0f;
		}

		if (dikeyboard[DIK_Q] & 0x80)
		{
			m_y_angle += 1.0f;
		}
		else if (dikeyboard[DIK_E] & 0x80)
		{
			m_y_angle -= 1.0f;
		}

		if (dikeyboard[DIK_Y] & 0x80)
		{
			m_x_offset += 0.05f;
		}
		else if (dikeyboard[DIK_U] & 0x80)
		{
			m_x_offset -= 0.05f;
		}

		if (dikeyboard[DIK_H] & 0x80)
		{
			m_y_offset += 0.05f;
		}
		else if (dikeyboard[DIK_J] & 0x80)
		{
			m_y_offset -= 0.05f;
		}

		if (dikeyboard[DIK_N] & 0x80)
		{
			m_z_offset += 0.05f;
		}
		else if (dikeyboard[DIK_M] & 0x80)
		{
			m_z_offset -= 0.05f;
		}
	}

}