#include "stdafx.h"
#include "FragmentProcessor.h"

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
		*out_color = *color;
	}
}