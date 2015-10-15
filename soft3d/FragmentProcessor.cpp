#include "soft3d.h"
#include "VertexProcessor.h"
#include "FragmentProcessor.h"

using namespace vmath;

namespace soft3d
{

	FragmentProcessor::FragmentProcessor()
	{
	}


	FragmentProcessor::~FragmentProcessor()
	{
	}

	void FragmentProcessor::Process()
	{
		//vec3 R = reflect(-fs_in.L, fs_in.N);
		//vec3 diffuse = vmath::max<float>(dot(fs_in.N, fs_in.L), 0.0f) * vec3(0.2f, 0.2f, 0.2f);
		//vec3 specular = pow(vmath::max<float>(dot(R, fs_in.V), 0.0f), 4.0f) * vec3(0.7f, 0.7f, 0.7f);
		//vec3 finalcolor = diffuse + specular + vec3(0.1f);
		*out_color = tex->Sampler2D(&fs_in.uv);// *&finalcolor;
		//*out_color = fs_in.color;
	}
}