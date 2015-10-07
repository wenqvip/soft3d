#pragma once
#include <memory>
#include "vmath.h"

namespace soft3d
{

	typedef unsigned short uint16;
	typedef unsigned int uint32;

	void uC2fC(uint32 color, vmath::vec3* colorf);
	uint32 fC2uC(const vmath::vec3& colorf);

}
#include "Soft3dPipeline.h"