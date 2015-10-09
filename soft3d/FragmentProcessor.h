#pragma once

namespace soft3d
{

	class FragmentProcessor
	{
	public:
		FragmentProcessor();
		virtual ~FragmentProcessor();

		virtual void Process();

		Color color;
		uint32* out_color;
		vmath::vec2 uv;
		const Texture* tex;
	};

}