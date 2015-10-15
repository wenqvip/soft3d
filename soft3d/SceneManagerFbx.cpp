#include "soft3d.h"
#include "SceneManagerFbx.h"
#include "FbxLoader.h"

using namespace std;
using namespace vmath;

namespace soft3d
{


	void SceneManagerFbx::InitScene(uint16 width, uint16 height)
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

		//vbo->m_mode = VertexBufferObject::RENDER_LINE;
		vbo->m_mode = VertexBufferObject::RENDER_TRIANGLE;
		vbo->m_cullMode = VertexBufferObject::CULL_NONE;
		Soft3dPipeline::Instance()->SetVBO(vbo);

		uint32 tex_data[] = {
			0xFFFFFF, 0x3FBCEF, 0xFFFFFF, 0x3FBCEF,
			0x3FBCEF, 0xFF00FF, 0x3FBCEF, 0xFFFFFF,
			0xFFFFFF, 0x3FBCEF, 0xFF00FF, 0x3FBCEF,
			0x3FBCEF, 0xFFFFFF, 0x3FBCEF, 0xFFFFFF,
		};

		shared_ptr<Texture> tex(new Texture());
		tex->CopyFromBuffer(tex_data, 4, 4);
		Soft3dPipeline::Instance()->SetTexture(tex);
	}

	void SceneManagerFbx::Update()
	{
		float aspect = (float)m_width / (float)m_height;
		mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
		mat4 view_matrix = lookat(vec3(0.0f, 0.0f, 3.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f));
		float factor = GetTickCount() / 50 % 360;
		mat4 mv_matrix = view_matrix * translate(0.0f, 0.0f, 0.0f) * scale(1.0f) * rotate(factor, vec3(1.0f, 0.0f, 0.0f));

		SetUniform(0, mv_matrix);
		SetUniform(1, proj_matrix);
		SetUniform(2, view_matrix);

		Soft3dPipeline::Instance()->Clear(0);
	}


}