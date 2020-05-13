#pragma once
#include "soft3d.h"
#include <wincodec.h>

namespace soft3d
{

	class TextureLoader
	{
	public:
		static TextureLoader& Instance() {
			if (s_instance == nullptr)
				s_instance = new TextureLoader();
			return *s_instance;
		}
		~TextureLoader();
		void LoadTexture(const wchar_t* filename);

		const uint32_t* GetData() const {
			return m_buffer;
		}
		uint32_t GetWidth() const {
			return m_width;
		}
		uint32_t GetHeight() const {
			return m_height;
		}

	protected:
		TextureLoader();

	private:
		IWICImagingFactory* m_pIWICFactory;
		uint32_t* m_buffer;
		uint32_t m_width;
		uint32_t m_height;

		static TextureLoader* s_instance;
	};

}