#include <assert.h>
#include "soft3d.h"
#include "VertexProcessor.h"

using namespace std;
using namespace vmath;

namespace soft3d
{
	void VertexProcessor::Process()
	{
		vec4 P = (*mv_matrix) * (*pos);
		vec3 N = mat3(*mv_matrix) * (*normal);
		vec3 L = vec3(0.0f, 0.0f, -100.0f) - P.xyz();
		vec3 V = -P.xyz();

		N = normalize(N);
		L = normalize(L);
		V = normalize(V);

		vec3 R = reflect(-L, N);

		vec3 diffuse = max<float>(dot(N, L), 0.0f) * vec3(0.3f, 0.3f, 0.3f);
		vec3 specular = pow(max<float>(dot(R, V), 0.0f), 64.0f) * vec3(0.5f, 0.5f, 0.5f);
		vec3 finalcolor = diffuse + specular + vec3(0.2f);

		*out_pos = (*proj_matrix) * P;
		*out_color = fC2uC(finalcolor);
	}

}