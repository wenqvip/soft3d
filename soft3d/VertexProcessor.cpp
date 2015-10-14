#include <assert.h>
#include "soft3d.h"
#include "VertexProcessor.h"

using namespace std;
using namespace vmath;

namespace soft3d
{
	void VertexProcessor::Process()
	{
		mat4* mv_matrix = (mat4*)(uniforms[0]);
		mat4* proj_matrix = (mat4*)(uniforms[1]);
		vec4 P = (*mv_matrix) * (*pos);
		vec3 N = mat3(*mv_matrix) * (*normal);
		vec3 L = vec3(100.0f, 100.0f, 0.0f) - P.xyz();
		vec3 V = -P.xyz();

		N = normalize(N);
		L = normalize(L);
		V = normalize(V);

		vec3 R = reflect(-L, N);

		vec3 diffuse = vmath::max<float>(dot(N, L), 0.0f) * vec3(0.2f, 0.2f, 0.2f);
		vec3 specular = pow(vmath::max<float>(dot(R, V), 0.0f), 32.0f) * vec3(0.7f, 0.7f, 0.7f);
		vec3 finalcolor = diffuse + specular + vec3(0.1f);

		vs_out.pos = (*proj_matrix) * P;
		vs_out.color = fC2uC(finalcolor);
	}

}