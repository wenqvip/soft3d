#pragma once

namespace soft3d
{

	class VertexProcessor
	{
	public:
		VertexProcessor();
		virtual ~VertexProcessor();

		virtual void Process();

		const vmath::vec4* pos;
		const uint32* color;
		vmath::vec4* out_pos;
		uint32* out_color;

		const vmath::mat4* mv_matrix;
		const vmath::mat4* proj_matrix;
	};

}