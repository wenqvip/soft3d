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

		this->mode = vo0->mode;
	}

	float VS_OUT::InterpolateRHW(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, float ratio0, float ratio1, float ratio2)
	{
		this->rhw = vo0->rhw * ratio0 + vo1->rhw * ratio1 + vo2->rhw * ratio2;
		return this->rhw;
	}

	bool VS_OUT::InterpolateZ(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, float ratio0, float ratio1, float ratio2)
	{
		this->pos[2] = vo0->pos[2] * ratio0 + vo1->pos[2] * ratio1 + vo2->pos[2] * ratio2;
		return this->pos[2] >= -1.0f && this->pos[2] <= 1.0f;
	}

	void VS_OUT::Interpolate(const VS_OUT* vo0, const VS_OUT* vo1, const VS_OUT* vo2, float ratio0, float ratio1, float ratio2)
	{
		this->color = vo0->color * ratio0 + vo1->color * ratio1 + vo2->color * ratio2;
		this->uv[0] = (vo0->uv[0] * ratio0 + vo1->uv[0] * ratio1 + vo2->uv[0] * ratio2) / this->rhw;
		this->uv[1] = (vo0->uv[1] * ratio0 + vo1->uv[1] * ratio1 + vo2->uv[1] * ratio2) / this->rhw;

		InterpolateT(this->L[0], vo0->L[0], vo1->L[0], vo2->L[0], ratio0, ratio1, ratio2);
		InterpolateT(this->L[1], vo0->L[1], vo1->L[1], vo2->L[1], ratio0, ratio1, ratio2);
		InterpolateT(this->L[2], vo0->L[2], vo1->L[2], vo2->L[2], ratio0, ratio1, ratio2);

		InterpolateT(this->N[0], vo0->N[0], vo1->N[0], vo2->N[0], ratio0, ratio1, ratio2);
		InterpolateT(this->N[1], vo0->N[1], vo1->N[1], vo2->N[1], ratio0, ratio1, ratio2);
		InterpolateT(this->N[2], vo0->N[2], vo1->N[2], vo2->N[2], ratio0, ratio1, ratio2);

		//InterpolateT(this->V[0], vo0->V[0], vo1->V[0], vo2->V[0], ratio0, ratio1, ratio2);
		//InterpolateT(this->V[1], vo0->V[1], vo1->V[1], vo2->V[1], ratio0, ratio1, ratio2);
		//InterpolateT(this->V[2], vo0->V[2], vo1->V[2], vo2->V[2], ratio0, ratio1, ratio2);

		InterpolateT(this->H[0], vo0->H[0], vo1->H[0], vo2->H[0], ratio0, ratio1, ratio2);
		InterpolateT(this->H[1], vo0->H[1], vo1->H[1], vo2->H[1], ratio0, ratio1, ratio2);
		InterpolateT(this->H[2], vo0->H[2], vo1->H[2], vo2->H[2], ratio0, ratio1, ratio2);

		this->mode = vo0->mode;
		this->triangleID = vo0->triangleID;
		this->instanceID = vo0->instanceID;
	}


	void VertexProcessor::Process()
	{
		mat4* mv_matrix = (mat4*)(uniforms[UNIFORM_MV_MATRIX]);
		mat4* proj_matrix = (mat4*)(uniforms[UNIFORM_PROJ_MATRIX]);
		vec3* light_pos = (vec3*)(uniforms[UNIFORM_LIGHT_POS]);
		vec3* light_dir = (vec3*)(uniforms[UNIFORM_LIGHT_DIR]);
		vec4 P = (*mv_matrix) * (*pos);
		if (normal != nullptr && (light_pos != nullptr || light_dir != nullptr))
		{
			vs_out.mode = VS_OUT::LIGHT_MODE;
			vs_out.N = mat3(*mv_matrix) * (*normal);
			if (light_dir != nullptr)
				vs_out.L = -*light_dir;
			else
				vs_out.L = *light_pos - P.xyz();
			vs_out.V = -P.xyz();
			vs_out.H = (vs_out.V + vs_out.L) / 2.0f;
		}
		else
		{
			vs_out.mode = VS_OUT::TEXTURE_MODE;
		}

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