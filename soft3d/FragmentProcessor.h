#pragma once

namespace soft3d
{

	class FragmentProcessor
	{
	public:
		FragmentProcessor();
		virtual ~FragmentProcessor();

		virtual void Process();

		vmath::vec4 color;
		vmath::vec4* out_color;
	};

}