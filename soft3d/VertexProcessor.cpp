#include "stdafx.h"
#include "VertexProcessor.h"
#include <assert.h>

using namespace std;
using namespace vmath;

namespace soft3d
{

	VertexProcessor::VertexProcessor()
	{
	}


	VertexProcessor::~VertexProcessor()
	{
	}

	void VertexProcessor::Process()
	{
		*out_pos = (*proj_matrix) * (*mv_matrix) * (*pos);
		*out_color = *color;
	}

}