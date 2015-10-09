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
		1.0f, 1.0f, 0.0f, 1.0f,
		0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f,
		0.0f,
		1.0f, -1.0f, 0.0f, 1.0f,
		0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f,
		0.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		0.0f,
	};
	(*(uint32*)&(cube[4])) = 0xff0000;
	(*(uint32*)&(cube[9])) = 0x00ff00;
	(*(uint32*)&(cube[14])) = 0x0000ff;
	(*(uint32*)&(cube[19])) = 0xff0000;
	(*(uint32*)&(cube[24])) = 0x00ff00;
	(*(uint32*)&(cube[29])) = 0x0000ff;

	float uv[] = {
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	shared_ptr<VertexBufferObject> vbo(new VertexBufferObject(5));
	vbo->CopyVertexBuffer(cube, sizeof(cube) / 20);
	vbo->m_posOffset = 0;
	vbo->m_colorOffset = 4;

	vbo->CopyUVBuffer(uv, sizeof(uv) / sizeof(float));

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
	Soft3dPipeline::Instance()->SetTexture(tex);
}

void SceneManagerTriangle::Update()
{
	float aspect = (float)m_width / (float)m_height;
	mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
	mat4 view_matrix = lookat(vec3(0.0f, 0.0f, 3.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	float factor = GetTickCount() / 50 % 360;
	mat4 mv_matrix = view_matrix * translate(0.0f, 0.0f, 0.0f) * scale(1.0f) * rotate(-30.0f, vec3(1.0f, 1.0f, 0.0f));

	Soft3dPipeline::Instance()->CurrentVBO()->mv_matrix = mv_matrix;
	Soft3dPipeline::Instance()->CurrentVBO()->proj_matrix = proj_matrix;

	Soft3dPipeline::Instance()->Clear(0);
}