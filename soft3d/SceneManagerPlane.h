#pragma once
#include "SceneManager.h"

namespace soft3d
{

	class SceneManagerPlane :
		public SceneManager
	{
	public:
		SceneManagerPlane() {};
		virtual ~SceneManagerPlane() {};
		virtual void InitScene(uint16_t width, uint16_t height);
		virtual void Update();
		void KeyboardEventCB(const DIKEYBOARD dikeyboard);

	private:
		float m_x_offset = 0.0f;
		float m_y_offset = 0.0f;
		float m_z_offset = 0.0f;
		float m_x_angle = 0.0f;
		float m_y_angle = 0.0f;
		float m_z_angle = 0.0f;
		int m_vbo1;
		int m_vbo2;
	};

}