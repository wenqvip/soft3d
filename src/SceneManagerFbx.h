#pragma once
#include "SceneManager.h"
#include "FbxLoader.h"

namespace soft3d
{

	class SceneManagerFbx :	public SceneManager
	{
	public:
		SceneManagerFbx() {};
		virtual ~SceneManagerFbx() {};
		virtual void InitScene(uint16_t width, uint16_t height);
		virtual void Update();
		void KeyboardEventCB(const DIKEYBOARD dikeyboard);

	private:
		float m_x_angle = -90.0f;
		float m_y_angle = 0.0f;
		float m_z_angle = 0.0f;

		float m_light_angle_y = -3.1415926f/2.0f;

		float m_x_offset = 0.0f;
		float m_y_offset = 0.0f;
		float m_z_offset = -6.0f;

		int m_vbo1;
		int m_vbo2;

		FbxLoader m_fbx;
	};

}