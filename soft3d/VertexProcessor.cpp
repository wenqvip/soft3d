#include <assert.h>
#include "soft3d.h"
#include "VertexProcessor.h"

using namespace std;
using namespace vmath;

namespace soft3d
{
	template<typename T>
	void InterpolateT(T& dst, const T& src0, const T& src1, const T& src2, float ratio0, float ratio1, float ratio2)
	{
		dst = src0 * ratio0 + src1 * ratio1 + src2 * ratio2;
	}

	void VS_OUT::Interpolate(const VS_OUT* vo0, const VS_OUT* vo1, float ratio0, float ratio1)
	{
		this->rhw = vo0->rhw * ratio0 + vo1->rhw * ratio1;
		this->color = vo0->color * ratio0 + vo1->color * ratio1;
		this->uv[0] = (vo0->uv[0] * ratio0 + vo1->uv[0] * ratio1) / this->rhw;
		this->uv[1] = (vo0->uv[1] * ratio0 + vo1->uv[1] * ratio1) / this->rhw;
	}

	void VS_OUT::Interpolate(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, float ratio0, float ratio1, float ratio2)
	{
		this->rhw = vo0->rhw * ratio0 + vo1->rhw * ratio1 + vo2->rhw * ratio2;
		this->color = vo0->color * ratio0 + vo1->color * ratio1 + vo2->color * ratio2;
		this->uv[0] = (vo0->uv[0] * ratio0 + vo1->uv[0] * ratio1 + vo2->uv[0] * ratio2) / this->rhw;
		this->uv[1] = (vo0->uv[1] * ratio0 + vo1->uv[1] * ratio1 + vo2->uv[1] * ratio2) / this->rhw;

		InterpolateT(this->L[0], vo0->L[0], vo1->L[0], vo2->L[0], ratio0, ratio1, ratio2);
		InterpolateT(this->L[1], vo0->L[1], vo1->L[1], vo2->L[1], ratio0, ratio1, ratio2);
		InterpolateT(this->L[2], vo0->L[2], vo1->L[2], vo2->L[2], ratio0, ratio1, ratio2);

		InterpolateT(this->N[0], vo0->N[0], vo1->N[0], vo2->N[0], ratio0, ratio1, ratio2);
		InterpolateT(this->N[1], vo0->N[1], vo1->N[1], vo2->N[1], ratio0, ratio1, ratio2);
		InterpolateT(this->N[2], vo0->N[2], vo1->N[2], vo2->N[2], ratio0, ratio1, ratio2);

		InterpolateT(this->V[0], vo0->V[0], vo1->V[0], vo2->V[0], ratio0, ratio1, ratio2);
		InterpolateT(this->V[1], vo0->V[1], vo1->V[1], vo2->V[1], ratio0, ratio1, ratio2);
		InterpolateT(this->V[2], vo0->V[2], vo1->V[2], vo2->V[2], ratio0, ratio1, ratio2);
	}


	void VertexProcessor::Process()
	{
		mat4* mv_matrix = (mat4*)(uniforms[0]);
		mat4* proj_matrix = (mat4*)(uniforms[1]);
		vec4 P = (*mv_matrix) * (*pos);
		//vs_out.N = mat3(*mv_matrix) * (*normal);
		//vs_out.L = vec3(100.0f, 100.0f, 0.0f) - P.xyz();
		//vs_out.V = -P.xyz();

		//vs_out.N = normalize(vs_out.N);
		//vs_out.L = normalize(vs_out.L);
		//vs_out.V = normalize(vs_out.V);

		//vec3 R = reflect(-vs_out.L, vs_out.N);

		//vec3 diffuse = vmath::max<float>(dot(vs_out.N, vs_out.L), 0.0f) * vec3(0.2f, 0.2f, 0.2f);
		//vec3 specular = pow(vmath::max<float>(dot(R, vs_out.V), 0.0f), 4.0f) * vec3(0.7f, 0.7f, 0.7f);
		//vec3 finalcolor = diffuse + specular + vec3(0.1f);

		vs_out.pos = (*proj_matrix) * P;
		//vs_out.color = fC2uC(finalcolor);
	}

}