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
		fbxLoader.LoadFbx("sphere.fbx");

		shared_ptr<VertexBufferObject> vbo(new VertexBufferObject());
		vbo->CopyVertexBuffer(fbxLoader.GetVertexBuffer(), fbxLoader.GetVertexCount() * 4);
		vbo->CopyIndexBuffer(fbxLoader.GetIndexBuffer(), fbxLoader.GetIndexCount());
		vbo->CopyNormalBuffer(fbxLoader.GetNormalBuffer(), fbxLoader.GetNormalCount() * 3);

		//vbo->m_mode = VertexBufferObject::RENDER_LINE;
		vbo->m_mode = VertexBufferObject::RENDER_TRIANGLE;
		Soft3dPipeline::Instance()->SetVBO(vbo);
	}

	void SceneManagerFbx::Update()
	{
		float aspect = (float)m_width / (float)m_height;
		mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
		mat4 view_matrix = lookat(vec3(0.0f, 0.0f, 3.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f));
		float factor = GetTickCount() / 50 % 360;
		mat4 mv_matrix = view_matrix * translate(0.0f, 0.0f, 0.0f) * scale(0.1f) * rotate(factor, vec3(0.0f, 1.0f, 0.0f));

		SetUniform(0, mv_matrix);
		SetUniform(1, proj_matrix);
		SetUniform(2, view_matrix);

		Soft3dPipeline::Instance()->Clear(0);
	}


}