#include "soft3d.h"
#include "SceneManagerTriangle.h"

using namespace soft3d;
using namespace std;
using namespace vmath;


SceneManagerTriangle::SceneManagerTriangle()
{
}


SceneManagerTriangle::~SceneManagerTriangle()
{
}

void SceneManagerTriangle::InitScene(uint16 width, uint16 height)
{
	m_width = width;
	m_height = height;

	float cube[] = {
		// 1.0f,  1.0f, 0.0f, 1.0f,
		//-0.0f, -0.0f, 0.0f, 1.0f,
		// 1.0f, -0.0f, 0.0f, 1.0f,

		// 0.0f,  1.0f, 0.0f, 1.0f,
		//-0.0f, -0.0f, 0.0f, 1.0f,
		// 1.0f,  1.0f, 0.0f, 1.0f,

		// 0.0f,  1.0f, 0.0f, 1.0f,
		//-1.0f,  0.0f, 0.0f, 1.0f,
		// 0.0f,  0.0f, 0.0f, 1.0f,

		//-1.0f,  1.0f, 0.0f, 1.0f,
		//-1.0f,  0.0f, 0.0f, 1.0f,
		// 0.0f,  1.0f, 0.0f, 1.0f,

		//-1.0f,  0.0f, 0.0f, 1.0f,
		//-1.0f, -1.0f, 0.0f, 1.0f,
		// 0.0f,  0.0f, 0.0f, 1.0f,

		// 0.0f,  0.0f, 0.0f, 1.0f,
		//-1.0f, -1.0f, 0.0f, 1.0f,
		// 0.0f, -1.0f, 0.0f, 1.0f,

		// 0.0f,  0.0f, 0.0f, 1.0f,
		// 0.0f, -1.0f, 0.0f, 1.0f,
		// 1.0f,  0.0f, 0.0f, 1.0f,

		// 1.0f,  0.0f, 0.0f, 1.0f,
		// 0.0f, -1.0f, 0.0f, 1.0f,
		// 1.0f, -1.0f, 0.0f, 1.0f,
		-1.000000, -1.000000, 0.000000, 1.000000,
		 0.000000, -1.000000, 0.000000, 1.000000,
		 1.000000, -1.000000, 0.000000, 1.000000,
		-1.000000,  0.000000, 0.000000, 1.000000,
		 0.000000,  0.000000, 0.000000, 1.000000,
		 1.000000,  0.000000, 0.000000, 1.000000,
		-1.000000,  1.000000, 0.000000, 1.000000,
		 0.000000,  1.000000, 0.000000, 1.000000,
		 1.000000,  1.000000, 0.000000, 1.000000,
	};

	uint32 cubeColor[] = {
		0xff0000,
		0x00ff00,
		0x0000ff,
		0xff0000,
		0x00ff00,
		0x0000ff,
		0xff0000,
		0x00ff00,
		0x0000ff,
		0xff0000,
		0x00ff00,
		0x0000ff,
		0xff0000,
		0x00ff00,
		0x0000ff,
		0xff0000,
		0x00ff00,
		0x0000ff,
		0xff0000,
		0x00ff00,
		0x0000ff,
		0xff0000,
		0x00ff00,
		0x0000ff,
	};

	float uv[] = {
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	float cubeNormal[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	uint32 cubeIndex[] = {
		3,0,4,
		1,4,0,
		4,1,5,
		2,5,1,
		6,3,7,
		4,7,3,
		7,4,8,
		5,8,4,
	};

	shared_ptr<VertexBufferObject> vbo(new VertexBufferObject());
	vbo->CopyVertexBuffer(cube, sizeof(cube) / sizeof(float));
	vbo->CopyColorBuffer(cubeColor, sizeof(cubeColor) / sizeof(uint32));
	//vbo->CopyUVBuffer(uv, sizeof(uv) / sizeof(float));
	vbo->CopyNormalBuffer(cubeNormal, sizeof(cubeNormal) / sizeof(float));
	vbo->CopyIndexBuffer(cubeIndex, sizeof(cubeIndex) / sizeof(uint32));

	vbo->m_mode = VertexBufferObject::RENDER_TRIANGLE;
	//vbo->m_mode = VertexBufferObject::RENDER_LINE;
	vbo->m_cullMode = VertexBufferObject::CULL_NONE;
	Soft3dPipeline::Instance()->SetVBO(vbo);

	uint32 tex_data[] = {
		0xff0000, 0x00ff00, 0x0000ff,
		0x00ff00, 0x0000ff, 0xff0000,
		0x0000ff, 0xff0000, 0x00ff00,
	};

	shared_ptr<Texture> tex(new Texture());
	tex->CopyFromBuffer(tex_data, 3, 3);
	//Soft3dPipeline::Instance()->SetTexture(tex);
}

void SceneManagerTriangle::Update()
{
	float aspect = (float)m_width / (float)m_height;
	mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
	mat4 view_matrix = lookat(vec3(0.0f, 0.0f, 3.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	float factor = GetTickCount() / 50 % 360;
	mat4 mv_matrix = view_matrix * translate(0.0f, 0.0f, 0.0f) * scale(1.0f) * rotate(factor, vec3(1.0f, 0.0f, 0.0f));

	Soft3dPipeline::Instance()->CurrentVBO()->mv_matrix = mv_matrix;
	Soft3dPipeline::Instance()->CurrentVBO()->proj_matrix = proj_matrix;

	Soft3dPipeline::Instance()->Clear(0);
}