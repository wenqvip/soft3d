#include <assert.h>
#include "soft3d.h"
#include "VertexProcessor.h"

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
		//*out_pos = (*pos) * 0.3f;
		//*out_pos = (*proj_matrix) * (*out_pos);
		//(*out_pos)[3] = 1.0f;
		*out_pos = (*proj_matrix) * (*mv_matrix) * (*pos);
		*out_color = *color;
	}

}