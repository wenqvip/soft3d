#include "stdafx.h"
#include "SceneManager.h"
#include "VertexBufferObject.h"
#include "vmath.h"
#include "Soft3dPipeline.h"
#include <time.h>

using namespace std;
using namespace vmath;

namespace soft3d
{
	shared_ptr<SceneManager> SceneManager::s_instance(new SceneManager());

	SceneManager::SceneManager()
	{
	}


	SceneManager::~SceneManager()
	{
	}


	shared_ptr<VertexBufferObject> vbo(new VertexBufferObject(5));
	void SceneManager::Init()
	{
		float cube[] = {
			1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			-1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			0.0f,
			-1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			-1.0f, -1.0f, 1.0f, 1.0f,
			0.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			0.0f,

			1.0f, 1.0f, -1.0f, 1.0f,
			0.0f,
			-1.0f, 1.0f, -1.0f, 1.0f,
			0.0f,
			1.0f, -1.0f, -1.0f, 1.0f,
			0.0f,
			-1.0f, 1.0f, -1.0f, 1.0f,
			0.0f,
			-1.0f, -1.0f, -1.0f, 1.0f,
			0.0f,
			1.0f, -1.0f, -1.0f, 1.0f,
			0.0f,
		};
		(*(uint32*)&(cube[4])) = 0xff0000;
		(*(uint32*)&(cube[9])) = 0x00ff00;
		(*(uint32*)&(cube[14])) = 0x0000ff;
		(*(uint32*)&(cube[19])) = 0x00ff00;
		(*(uint32*)&(cube[24])) = 0xff0000;
		(*(uint32*)&(cube[29])) = 0x0000ff;

		(*(uint32*)&(cube[34])) = 0xffff00;
		(*(uint32*)&(cube[39])) = 0xff00ff;
		(*(uint32*)&(cube[44])) = 0x00ffff;
		(*(uint32*)&(cube[49])) = 0xff00ff;
		(*(uint32*)&(cube[54])) = 0xffff00;
		(*(uint32*)&(cube[59])) = 0x00ffff;

		vbo->CopyFromBuffer(cube, sizeof(cube)/20);
		vbo->m_posOffset = 0;
		vbo->m_colorOffset = 4;
	}

	void SceneManager::Update()
	{
		float aspect = (float)DirectXHelper::Instance()->GetWidth() / (float)DirectXHelper::Instance()->GetHeight();
		mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
		mat4 view_matrix = lookat(vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f));
		float factor = GetTickCount() / 10 % 360;
		mat4 mv_matrix = translate(0.0f, 0.0f, -10.0f) * scale(2.0f) * rotate(factor, vec3(0.0f, 1.0f, 0.0f));

		vbo->mv_matrix = mv_matrix;
		vbo->proj_matrix = proj_matrix;

		Soft3dPipeline::Instance()->SetVBO(vbo);
	}

}