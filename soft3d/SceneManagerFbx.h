#pragma once
#include "SceneManager.h"

namespace soft3d
{

	class SceneManagerFbx :	public SceneManager
	{
	public:
		SceneManagerFbx() {};
		virtual ~SceneManagerFbx() {};
		virtual void InitScene(soft3d::uint16 width, soft3d::uint16 height);
		virtual void Update();
		void KeyboardEventCB(const DIKEYBOARD dikeyboard);

	private:
		float m_x_angle = 90.0f;
		float m_y_angle = 0.0f;
		float m_z_angle = 0.0f;
	};

}