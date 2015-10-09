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
		*out_color = tex->Sampler2D(&uv);
	}
}