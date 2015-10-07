#include "soft3d.h"
#include "VertexProcessor.h"
#include "FragmentProcessor.h"

using namespace vmath;

namespace soft3d
{

	FragmentProcessor::FragmentProcessor()
	{
	}


	FragmentProcessor::~FragmentProcessor()
	{
	}

	void FragmentProcessor::Process()
	{
		//color = vec3(1.0f);
		*out_color = fC2uC(color);
	}
}