#pragma once
#include "VertexProcessor.h"

namespace soft3d
{

	class FragmentProcessor
	{
	public:
		FragmentProcessor();
		virtual ~FragmentProcessor();

		virtual void Process();

		const uint32* color;
		uint32* out_color;
	};

}