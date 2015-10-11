#include <assert.h>
#include "soft3d.h"
#include "VertexProcessor.h"

using namespace std;
using namespace vmath;

namespace soft3d
{
	void VertexProcessor::Process()
	{
		vec4 pos_vs = (*mv_matrix) * (*pos);
		*out_pos = (*proj_matrix) * pos_vs;
		*out_color = *color;
	}

}