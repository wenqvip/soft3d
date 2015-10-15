#pragma once

namespace soft3d
{

	class FragmentProcessor
	{
	public:
		FragmentProcessor();
		virtual ~FragmentProcessor();

		virtual void Process();

		VS_OUT fs_in;
		uint32* out_color;
		const Texture* tex;
	};

}