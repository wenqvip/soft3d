#pragma once

namespace soft3d
{

	typedef void* UniformPtr;

	struct VS_OUT
	{
		enum MODE
		{
			LIGHT_MODE,
			COLOR_MODE,
			TEXTURE_MODE,
		};
		vmath::vec4 pos;
		Color color = Color::purple;
		const vmath::vec3 normal;
		vmath::vec2 uv;
		float rhw;

		vmath::vec3 N;
		vmath::vec3 L;
		vmath::vec3 V;
		vmath::vec3 H;
		MODE mode = LIGHT_MODE;

		uint32_t vertexID = 0xffffffff;
		uint32_t triangleID = 0;
		uint32_t instanceID = 0;

		void Interpolate(const VS_OUT* vo0, const VS_OUT* vo1, float ratio0, float ratio1);
		void Interpolate(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, float ratio0, float ratio1, float ratio2);
		float InterpolateRHW(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, float ratio0, float ratio1, float ratio2);
		bool InterpolateZ(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, float ratio0, float ratio1, float ratio2);
	};

	struct VertexProcessor
	{
		virtual void Process();

		const vmath::vec4* pos = nullptr;
		const uint32_t* color = nullptr;
		const vmath::vec3* normal = nullptr;

		VS_OUT vs_out;
		UniformPtr* uniforms = nullptr;
	};

}