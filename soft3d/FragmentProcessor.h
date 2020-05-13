#pragma once

namespace soft3d
{

	class FragmentProcessor
	{
	public:
		FragmentProcessor() = default;
		virtual ~FragmentProcessor() = default;

		virtual void Process();

		VS_OUT fs_in;
		uint32_t* out_color = nullptr;
		const Texture* tex = nullptr;
	};

}