#pragma once
#include "SceneManager.h"

namespace soft3d
{

	class SceneManagerFbx :
		public SceneManager
	{
	public:
		SceneManagerFbx() {};
		virtual ~SceneManagerFbx() {};
		virtual void InitScene(soft3d::uint16 width, soft3d::uint16 height);
		virtual void Update();
	};

}