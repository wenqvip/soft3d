#pragma once
#include "SceneManager.h"

class SceneManagerTriangle : public soft3d::SceneManager
{
public:
	SceneManagerTriangle();
	virtual ~SceneManagerTriangle();
	virtual void InitScene(soft3d::uint16 width, soft3d::uint16 height);
	virtual void Update();
};

