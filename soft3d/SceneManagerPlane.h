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
		virtual void InitScene(soft3d::uint16 width, soft3d::uint16 height);
		virtual void Update();
	};

}