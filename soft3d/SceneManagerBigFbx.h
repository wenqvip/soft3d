#pragma once
#include "SceneManager.h"
#include "FbxLoader.h"

namespace soft3d
{

	class SceneManagerBigFbx :	public SceneManager
	{
	public:
		SceneManagerBigFbx() {};
		virtual ~SceneManagerBigFbx() {};
		virtual void InitScene(soft3d::uint16 width, soft3d::uint16 height);
		virtual void Update();
		void KeyboardEventCB(const DIKEYBOARD dikeyboard);

	private:
		float m_x_angle = -90.0f;
		float m_y_angle = 0.0f;
		float m_z_angle = 0.0f;

		float m_light_angle_y = -3.1415926f/2.0f;

		float m_x_offset = 0.0f;
		float m_y_offset = -174.0f;
		float m_z_offset = -280.0f;

		int m_vbo1;
		int m_vbo2;

		FbxLoader m_fbx;
	};

}