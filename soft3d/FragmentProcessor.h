#pragma once

namespace soft3d
{

	class FragmentProcessor
	{
	public:
		FragmentProcessor();
		virtual ~FragmentProcessor();

		virtual void Process();

		uint32 color;
		uint32* out_color;
	};

}