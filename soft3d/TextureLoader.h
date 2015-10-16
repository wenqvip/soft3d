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

		const uint32* GetData() const {
			return m_buffer;
		}
		uint32 GetWidth() const {
			return m_width;
		}
		uint32 GetHeight() const {
			return m_height;
		}

	protected:
		TextureLoader();

	private:
		IWICImagingFactory* m_pIWICFactory;
		uint32* m_buffer;
		uint32 m_width;
		uint32 m_height;

		static TextureLoader* s_instance;
	};

}