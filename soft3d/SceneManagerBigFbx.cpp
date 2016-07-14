#include "soft3d.h"
#include "SceneManagerBigFbx.h"
#include "TextureLoader.h"
#include <boost/bind.hpp>

using namespace std;
using namespace vmath;

namespace soft3d
{


	void SceneManagerBigFbx::InitScene(uint16 width, uint16 height)
	{
		m_width = width;
		m_height = height;

		m_fbx.LoadFbx("zhankuang.fbx");

		shared_ptr<VertexBufferObject> vbo(new VertexBufferObject());
		vbo->CopyVertexBuffer(m_fbx.GetVertexBuffer(), m_fbx.GetVertexCount() * 4);
		vbo->CopyIndexBuffer(m_fbx.GetIndexBuffer(), m_fbx.GetIndexCount());
		vbo->CopyNormalBuffer(m_fbx.GetNormalBuffer(), m_fbx.GetNormalCount() * 3);
		vbo->CopyUVBuffer(m_fbx.GetUVBuffer(), m_fbx.GetUVCount() * 2);

		//vbo->m_cullMode = VertexBufferObject::CULL_NONE;
		//vbo->m_mode = VertexBufferObject::RENDER_LINE;
		vbo->m_mode = VertexBufferObject::RENDER_TRIANGLE;
		m_vbo1 = Soft3dPipeline::Instance()->SetVBO(vbo);

		TextureLoader::Instance().LoadTexture(L"zhankuang.png");

		shared_ptr<Texture> tex(new Texture());
		tex->CopyFromBuffer(TextureLoader::Instance().GetData(), TextureLoader::Instance().GetWidth(), TextureLoader::Instance().GetHeight());
		Soft3dPipeline::Instance()->SetTexture(tex);

		Soft3dPipeline::Instance()->AddKeyboardEventCB(boost::bind(&SceneManagerBigFbx::KeyboardEventCB, this, _1));
	}

	void SceneManagerBigFbx::Update()
	{
		double time = GetTickCount() / 1000.0;
		FbxAMatrix& fmat = m_fbx.GetRootMatrixLocalAtTime(time);
		mat4 anim_mat;
		for (int m = 0; m < 4; m++)
			for (int n = 0; n < 4; n++)
			{
				anim_mat[m][n] = fmat.mData[m][n];
			}

		float aspect = (float)m_width / (float)m_height;
		mat4 proj_matrix = perspective(30.0f, aspect, 0.1f, 1000.0f);
		mat4 view_matrix = lookat(vec3(0.0f, 0.0f, 3.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f));
		float factor = GetTickCount() / 50 % 360;

		float x = cos(m_light_angle_y) * 50000;
		float z = sin(m_light_angle_y) * 50000;

		Soft3dPipeline::Instance()->SelectVBO(m_vbo1);
		mat4 mv_matrix = view_matrix
			* translate(1.1f + m_x_offset, 0.0f + m_y_offset, -0.5f + m_z_offset)
			* scale(1.0f)
			* rotate(m_x_angle, vec3(1.0f, 0.0f, 0.0f))
			* rotate(m_y_angle, vec3(0.0f, 1.0f, 0.0f))
			* rotate(m_z_angle, vec3(0.0f, 0.0f, 1.0f));

		SetUniform(UNIFORM_MV_MATRIX, mv_matrix);
		SetUniform(UNIFORM_PROJ_MATRIX, proj_matrix);
		SetUniform(UNIFORM_VIEW_MATRIX, view_matrix);
		SetUniform(UNIFORM_LIGHT_DIR, vec3(x, 0.0f, z));
		
		Soft3dPipeline::Instance()->Clear(0);
	}

	void SceneManagerBigFbx::KeyboardEventCB(const DIKEYBOARD dikeyboard)
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

		if (dikeyboard[DIK_LEFT] & 0x80)
		{
			m_light_angle_y += 0.05f;
		}
		else if (dikeyboard[DIK_RIGHT] & 0x80)
		{
			m_light_angle_y -= 0.05f;
		}

		if (dikeyboard[DIK_Y] & 0x80)
		{
			m_x_offset += 2.0f;
		}
		else if (dikeyboard[DIK_U] & 0x80)
		{
			m_x_offset -= 2.0f;
		}

		if (dikeyboard[DIK_H] & 0x80)
		{
			m_y_offset += 2.0f;
		}
		else if (dikeyboard[DIK_J] & 0x80)
		{
			m_y_offset -= 2.0f;
		}

		if (dikeyboard[DIK_N] & 0x80)
		{
			m_z_offset += 4.0f;
		}
		else if (dikeyboard[DIK_M] & 0x80)
		{
			m_z_offset -= 4.0f;
		}
	}
}