#include "soft3d.h"
#include "SceneManagerTriangle.h"
#include <functional>

using namespace std;
using namespace vmath;

namespace soft3d
{


SceneManagerTriangle::SceneManagerTriangle()
{
}


SceneManagerTriangle::~SceneManagerTriangle()
{
}

void SceneManagerTriangle::InitScene(uint16_t width, uint16_t height)
{
	m_width = width;
	m_height = height;

	float cube[] = {
		-1, -1, 0, 1,
		-1,  1, 0, 1,
		 1,  1, 0, 1,
		 1, -1, 0, 1,

		-1.5, -1.5, -1, 1,
		-1.5,  1.5, -1, 1,
		 1.5,  1.5, -1, 1,
		 1.5, -1.5, -1, 1,
	};

	//uint32_t cubeColor[] = {
	//	0xff0000,
	//	0x00ff00,
	//	0x0000ff,
	//	0xff0000,
	//};

	float uv[] = {
		0.0f, 1.0f,
		0.5f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		0.5f, 1.0f,
		0.5f, 0.0f,

		0.5f, 1.0f,
		1.0f, 0.0f,
		0.5f, 0.0f,
		0.5f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};

	float cubeNormal[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	uint32_t cubeIndex[] = {
		0, 2, 1,
		0, 3, 2,
		//4, 6, 5,
		//4, 7, 6,
	};

	shared_ptr<VertexBufferObject> vbo(new VertexBufferObject());
	vbo->CopyVertexBuffer(cube, sizeof(cube) / sizeof(float));
	//vbo->CopyColorBuffer(cubeColor, sizeof(cubeColor) / sizeof(uint32_t));
	vbo->CopyUVBuffer(uv, sizeof(uv) / sizeof(float));
	//vbo->CopyNormalBuffer(cubeNormal, sizeof(cubeNormal) / sizeof(float));
	vbo->CopyIndexBuffer(cubeIndex, sizeof(cubeIndex) / sizeof(uint32_t));

	vbo->m_mode = VertexBufferObject::RENDER_TRIANGLE;
	//vbo->m_mode = VertexBufferObject::RENDER_LINE;
	vbo->m_cullMode = VertexBufferObject::CULL_NONE;
	Soft3dPipeline::Instance()->SetVBO(vbo);

	uint32_t tex_data[] = {
		0xa7dbff, 0xefed7a, 0xb8ecff,  0x136888, 0xeb5f25, 0x136999,
		0xefed7a, 0xb8ecff, 0xefed7a,  0xeb5f25, 0x136999, 0xeb5f25,
		0xb8ecff, 0xefed7a, 0xb8ecff,  0x136999, 0xeb5f25, 0x136999,
	};

	shared_ptr<Texture> tex(new Texture());
	tex->CopyFromBuffer(tex_data, 6, 3);
	tex->filter_mode = Texture::NEAREST;
	Soft3dPipeline::Instance()->SetTexture(tex);

	Soft3dPipeline::Instance()->AddKeyboardEventCB(std::bind(&SceneManagerTriangle::KeyboardEventCB, this, std::placeholders::_1));
}

void SceneManagerTriangle::Update()
{
	float aspect = (float)m_width / (float)m_height;
	mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
	mat4 view_matrix = lookat(vec3(0.0f, 0.0f, 3.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	float factor = 0;// GetTickCount() / 50 % 360;
	mat4 mv_matrix = view_matrix
		* translate(m_x_offset, m_y_offset, m_z_offset)
		* scale(1.0f)
		* rotate(m_x_angle, vec3(1.0f, 0.0f, 0.0f))
		* rotate(m_y_angle, vec3(0.0f, 1.0f, 0.0f))
		* rotate(m_z_angle, vec3(0.0f, 0.0f, 1.0f));

	SetUniform(0, mv_matrix);
	SetUniform(1, proj_matrix);

	Soft3dPipeline::Instance()->Clear(0xffffff);
}

void SceneManagerTriangle::KeyboardEventCB(const DIKEYBOARD dikeyboard)
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