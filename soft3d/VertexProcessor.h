#pragma once

namespace soft3d
{

	class VertexProcessor
	{
	public:
		VertexProcessor() {};
		virtual ~VertexProcessor() {};

		virtual void Process();

		const vmath::vec4* pos;
		const uint32* color;
		const vmath::vec3* normal;

		vmath::vec4* out_pos;
		Color* out_color;
		const vmath::vec3* out_normal;

		const vmath::mat4* mv_matrix;
		const vmath::mat4* proj_matrix;
	};

}