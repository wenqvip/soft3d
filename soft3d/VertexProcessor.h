#pragma once

namespace soft3d
{

	typedef void* UniformPtr;

	struct VS_OUT
	{
		vmath::vec4 pos;
		Color color;
		const vmath::vec3 normal;
		vmath::vec2 uv;
		float rhw;
	};

	struct VertexProcessor
	{
		virtual void Process();

		const vmath::vec4* pos;
		const uint32* color;
		const vmath::vec3* normal;

		VS_OUT vs_out;
		UniformPtr* uniforms;
	};

}