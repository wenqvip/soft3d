#pragma once

namespace soft3d
{

	class FragmentProcessor
	{
	public:
		FragmentProcessor();
		virtual ~FragmentProcessor();

		virtual void Process();

		vmath::vec3 color;
		uint32* out_color;
	};

}