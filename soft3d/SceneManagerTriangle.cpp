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
		0.0f, 0.732f, 0.0f, 1.0f,
		0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f,
		0.0f,
		1.0f, -1.0f, 0.0f, 1.0f,
		0.0f,
	};
	(*(uint32*)&(cube[4])) = 0xff0000;
	(*(uint32*)&(cube[9])) = 0x00ff00;
	(*(uint32*)&(cube[14])) = 0x0000ff;

	shared_ptr<VertexBufferObject> vbo(new VertexBufferObject(5));
	vbo->CopyFromBuffer(cube, sizeof(cube) / 20);
	vbo->m_posOffset = 0;
	vbo->m_colorOffset = 4;

	vbo->m_mode = VertexBufferObject::RENDER_TRIANGLE;
	//vbo->m_mode = VertexBufferObject::RENDER_LINE;
	Soft3dPipeline::Instance()->SetVBO(vbo);
}

void SceneManagerTriangle::Update()
{
	float aspect = (float)m_width / (float)m_height;
	mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
	mat4 view_matrix = lookat(vec3(0.0f, 0.0f, 3.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	mat4 mv_matrix = view_matrix * translate(0.0f, 0.0f, 0.0f) * scale(1.0f);

	Soft3dPipeline::Instance()->CurrentVBO()->mv_matrix = mv_matrix;
	Soft3dPipeline::Instance()->CurrentVBO()->proj_matrix = proj_matrix;

	Soft3dPipeline::Instance()->Clear(0);
}