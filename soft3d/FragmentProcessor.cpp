#include "soft3d.h"
#include "VertexProcessor.h"
#include "FragmentProcessor.h"

using namespace vmath;

namespace soft3d
{

	void FragmentProcessor::Process()
	{
		if (fs_in.mode == VS_OUT::LIGHT_MODE)
		{
			fs_in.N = normalize(fs_in.N);
			fs_in.L = normalize(fs_in.L);
			//fs_in.V = normalize(fs_in.V);
			fs_in.H = normalize(fs_in.H);

			//vec3 R = reflect(fs_in.L, fs_in.N);
			vec3 diffuse = vmath::max<float>(dot(fs_in.N, fs_in.L), 0.0f) * vec3(0.8f);
			vec3 specular = pow(vmath::max<float>(dot(fs_in.H, fs_in.N), 0.0f), 128.0f) * vec3(0.8f);
			vec3 finalcolor = diffuse + specular + vec3(0.1);
			if (tex)
				*out_color = tex->Sampler2D(&fs_in.uv) * (&finalcolor);
			else
				*out_color = Color(0xffffff) * &finalcolor;
		}
		else
		{
			if (tex != nullptr)
				*out_color = tex->Sampler2D(&fs_in.uv);
			else
				*out_color = fs_in.color;
		}
		//*out_color = fs_in.color;
	}
}