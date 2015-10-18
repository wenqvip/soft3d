#include "soft3d.h"
#include "SceneManagerFbx.h"
#include "FbxLoader.h"
#include "TextureLoader.h"
#include <boost/bind.hpp>

using namespace std;
using namespace vmath;

namespace soft3d
{


	void SceneManagerFbx::InitScene(uint16 width, uint16 height)
	{
		m_width = width;
		m_height = height;

		FbxLoader fbxLoader;
		fbxLoader.LoadFbx("sphere.fbx");

		shared_ptr<VertexBufferObject> vbo(new VertexBufferObject());
		vbo->CopyVertexBuffer(fbxLoader.GetVertexBuffer(), fbxLoader.GetVertexCount() * 4);
		vbo->CopyIndexBuffer(fbxLoader.GetIndexBuffer(), fbxLoader.GetIndexCount());
		vbo->CopyNormalBuffer(fbxLoader.GetNormalBuffer(), fbxLoader.GetNormalCount() * 3);
		vbo->CopyUVBuffer(fbxLoader.GetUVBuffer(), fbxLoader.GetUVCount() * 2);

		//vbo->m_mode = VertexBufferObject::RENDER_LINE;
		vbo->m_mode = VertexBufferObject::RENDER_TRIANGLE;
		//vbo->m_cullMode = VertexBufferObject::CULL_NONE;
		m_vbo1 = Soft3dPipeline::Instance()->SetVBO(vbo);
		m_vbo2 = Soft3dPipeline::Instance()->SetVBO(vbo);

		//uint32 tex_data[] = {
		//	0xFFFFFF, 0x3FBCEF, 0xFFFFFF, 0x3FBCEF,
		//	0x3FBCEF, 0xFFFFFF, 0x3FBCEF, 0xFFFFFF,
		//	0xFFFFFF, 0x3FBCEF, 0xFFFFFF, 0x3FBCEF,
		//	0x3FBCEF, 0xFFFFFF, 0x3FBCEF, 0xFFFFFF,
		//};
		TextureLoader::Instance().LoadTexture(L"earthmap.jpg");

		shared_ptr<Texture> tex(new Texture());
		tex->CopyFromBuffer(TextureLoader::Instance().GetData(), TextureLoader::Instance().GetWidth(), TextureLoader::Instance().GetHeight());
		Soft3dPipeline::Instance()->SetTexture(tex);

		Soft3dPipeline::Instance()->AddKeyboardEventCB(boost::bind(&SceneManagerFbx::KeyboardEventCB, this, _1));
	}

	void SceneManagerFbx::Update()
	{
		float aspect = (float)m_width / (float)m_height;
		mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
		mat4 view_matrix = lookat(vec3(0.0f, 0.0f, 3.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f));
		float factor = GetTickCount() / 50 % 360;

		Soft3dPipeline::Instance()->SelectVBO(m_vbo1);
		mat4 mv_matrix = view_matrix
			* translate(0.0f, 0.0f, 0.0f)
			* scale(1.0f)
			* rotate(m_x_angle, vec3(1.0f, 0.0f, 0.0f))
			* rotate(m_y_angle, vec3(0.0f, 1.0f, 0.0f))
			* rotate(m_z_angle, vec3(0.0f, 0.0f, 1.0f));

		SetUniform(0, mv_matrix);
		SetUniform(1, proj_matrix);
		SetUniform(2, view_matrix);

		Soft3dPipeline::Instance()->SelectVBO(m_vbo2);
		mv_matrix = view_matrix
			* translate(-1.0f, -1.0f, -1.0f)
			* scale(1.0f)
			* rotate(m_x_angle, vec3(1.0f, 0.0f, 0.0f))
			* rotate(m_y_angle, vec3(0.0f, 1.0f, 0.0f))
			* rotate(m_z_angle, vec3(0.0f, 0.0f, 1.0f));

		SetUniform(0, mv_matrix);
		SetUniform(1, proj_matrix);
		SetUniform(2, view_matrix);

		Soft3dPipeline::Instance()->Clear(0);
	}

	void SceneManagerFbx::KeyboardEventCB(const DIKEYBOARD dikeyboard)
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
	}
}