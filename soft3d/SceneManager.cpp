#include "soft3d.h"
#include "SceneManager.h"
#include "SceneManagerTriangle.h"
#include "SceneManagerFbx.h"
#include "SceneManagerPlane.h"

using namespace std;
using namespace vmath;
namespace soft3d
{

	shared_ptr<SceneManager> SceneManager::s_instance(new SceneManagerFbx());

	SceneManager::SceneManager()
	{
	}


	SceneManager::~SceneManager()
	{
	}


	void SceneManager::InitScene(uint16 width, uint16 height)
	{
		m_width = width;
		m_height = height;

		float cube[] = {
			1.0f, 1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 1.0f,
		};

		uint32 cubeColor[] = {
			0xff0000,
			0x00ff00,
			0x0000ff,
			0xff0000,
			0xffff00,
			0xff00ff,
			0x00ffff,
			0xffff00,
		};

		uint32 cubeIndex[] = {
			0, 1, 2,
			1, 3, 2,
			4, 5, 0,
			5, 1, 0,
			6, 7, 4,
			4, 7, 5,
			2, 3, 6,
			3, 7, 6,
			6, 4, 0,
			0, 2, 6,
			1, 5, 7,
			7, 3, 1,
		};

		float cubeuv[] = {
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		};

		shared_ptr<VertexBufferObject> vbo(new VertexBufferObject());
		vbo->CopyVertexBuffer(cube, sizeof(cube) / sizeof(float));
		vbo->CopyColorBuffer(cubeColor, sizeof(cubeColor) / sizeof(uint32));
		vbo->CopyIndexBuffer(cubeIndex, sizeof(cubeIndex) / sizeof(uint32));
		vbo->CopyUVBuffer(cubeuv, sizeof(cubeuv)/sizeof(float));

		vbo->m_mode = VertexBufferObject::RENDER_TRIANGLE;
		//vbo->m_mode = VertexBufferObject::RENDER_LINE;
		Soft3dPipeline::Instance()->SetVBO(vbo);

		uint32 tex_data[] = {
			0xFFFFFF, 0x3FBCEF, 0xFFFFFF, 0x3FBCEF,
			0x3FBCEF, 0xFFFFFF, 0x3FBCEF, 0xFFFFFF,
			0xFFFFFF, 0x3FBCEF, 0xFFFFFF, 0x3FBCEF,
			0x3FBCEF, 0xFFFFFF, 0x3FBCEF, 0xFFFFFF,
		};

		shared_ptr<Texture> tex(new Texture());
		tex->CopyFromBuffer(tex_data, 4, 4);
		Soft3dPipeline::Instance()->SetTexture(tex);
	}

	void SceneManager::Update()
	{
		float aspect = (float)m_width / (float)m_height;
		mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
		mat4 view_matrix = lookat(vec3(0.0f, 2.0f, 4.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f));
		float factor = GetTickCount() / 10 % 360;
		mat4 model_matrix = rotate(factor, vec3(0.0f, 1.0f, 0.0f)) * translate(0.0f, 0.0f, 0.0f) * scale(1.0f);
		mat4 mv_matrix = view_matrix * model_matrix;

		Soft3dPipeline::Instance()->CurrentVBO()->mv_matrix = mv_matrix;
		Soft3dPipeline::Instance()->CurrentVBO()->proj_matrix = proj_matrix;

		Soft3dPipeline::Instance()->Clear(0);
	}

}