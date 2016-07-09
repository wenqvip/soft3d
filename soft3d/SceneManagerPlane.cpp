#include "soft3d.h"
#include "SceneManagerPlane.h"
#include "TextureLoader.h"
#include "FbxLoader.h"
#include <boost/bind.hpp>

using namespace std;
using namespace vmath;

namespace soft3d
{


	void SceneManagerPlane::InitScene(uint16 width, uint16 height)
	{
		m_width = width;
		m_height = height;
		m_z = 0.0f;

		FbxLoader fbxLoader;
		fbxLoader.LoadFbx("plane2x2.fbx");

		shared_ptr<VertexBufferObject> vbo(new VertexBufferObject());
		vbo->CopyVertexBuffer(fbxLoader.GetVertexBuffer(), fbxLoader.GetVertexCount() * 4);
		vbo->CopyIndexBuffer(fbxLoader.GetIndexBuffer(), fbxLoader.GetIndexCount());
		vbo->CopyNormalBuffer(fbxLoader.GetNormalBuffer(), fbxLoader.GetNormalCount() * 3);
		vbo->CopyUVBuffer(fbxLoader.GetUVBuffer(), fbxLoader.GetUVCount() * 2);

		//vbo->m_mode = VertexBufferObject::RENDER_LINE;
		vbo->m_mode = VertexBufferObject::RENDER_TRIANGLE;
		vbo->m_cullMode = VertexBufferObject::CULL_NONE;
		Soft3dPipeline::Instance()->SetVBO(vbo);

		shared_ptr<Texture> tex(new Texture());
		TextureLoader::Instance().LoadTexture(L"cathead_small.png");
		tex->CopyFromBuffer(TextureLoader::Instance().GetData(), TextureLoader::Instance().GetWidth(), TextureLoader::Instance().GetHeight());
		//tex->filter_mode = Texture::NEAREST;

		//uint32 tex_data[] = {
		//	0xFFFFFF, 0x000000, 0xFFFFFF, 0x000000,
		//	0x000000, 0xFFFFFF, 0x000000, 0xFFFFFF,
		//	0xFFFFFF, 0x000000, 0xFFFFFF, 0x000000,
		//	0x000000, 0xFFFFFF, 0x000000, 0xFFFFFF,
		//};
		//tex->CopyFromBuffer(tex_data, 4, 4);
		Soft3dPipeline::Instance()->SetTexture(tex);

		Soft3dPipeline::Instance()->AddKeyboardEventCB(boost::bind(&SceneManagerPlane::KeyboardEventCB, this, _1));
	}

	void SceneManagerPlane::Update()
	{
		float aspect = (float)m_width / (float)m_height;
		mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
		mat4 view_matrix = lookat(vec3(0.0f, 0.0f, 3.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f));
		float factor = -180;// ((int)GetTickCount() / 50 % 180) - 90;
		mat4 mv_matrix = view_matrix * translate(0.0f, 0.0f, m_z) * scale(1.0f) *rotate(factor, vec3(1.0f, 0.0f, 0.0f));

		SetUniform(UNIFORM_MV_MATRIX, mv_matrix);
		SetUniform(UNIFORM_PROJ_MATRIX, proj_matrix);
		SetUniform(UNIFORM_LIGHT_POS, vec3(0.0f, 0.0f, -100.0f));

		Soft3dPipeline::Instance()->Clear(0);
	}

	void SceneManagerPlane::KeyboardEventCB(const DIKEYBOARD dikeyboard)
	{
		if (dikeyboard[DIK_W] & 0x80)
		{
			m_z += 0.1f;
		}
		else if (dikeyboard[DIK_S] & 0x80)
		{
			m_z -= 0.1f;
		}
	}

}