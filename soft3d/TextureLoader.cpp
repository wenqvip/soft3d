#include "TextureLoader.h"
#pragma comment(lib, "windowscodecs.lib")

namespace soft3d
{
	TextureLoader* TextureLoader::s_instance = nullptr;

	void ThrowIfFailed(HRESULT hr) {
		if (FAILED(hr))
		{
			throw "Error";
		}
	}

	TextureLoader::TextureLoader()
	{
		HRESULT hr;
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pIWICFactory));
	}

	TextureLoader::~TextureLoader()
	{
		m_pIWICFactory->Release();
	}

	void TextureLoader::LoadTexture(const wchar_t* filename)
	{
		HRESULT hr = S_OK;

		IWICBitmapDecoder *pIDecoder = NULL;
		IWICBitmapFrameDecode *pIDecoderFrame = NULL;
		UINT nFrameCount = 0;

		// Create decoder for an image.
		hr = m_pIWICFactory->CreateDecoderFromFilename(
			filename,                  // Image to be decoded
			NULL,                           // Do not prefer a particular vendor
			GENERIC_READ,                   // Desired read access to the file
			WICDecodeMetadataCacheOnDemand, // Cache metadata when needed
			&pIDecoder                      // Pointer to the decoder
			);

			// Retrieve the next bitmap frame.
		if (SUCCEEDED(hr))
		{
			hr = pIDecoder->GetFrame(0, &pIDecoderFrame);
		}

		// Retrieve the size of the bitmap frame.
		if (SUCCEEDED(hr))
		{
			hr = pIDecoderFrame->GetSize(&m_width, &m_height);
		}

		IWICBitmap *pIBitmap = NULL;
		IWICBitmapLock *pILock = NULL;

		// Create the bitmap from the image frame.
		if (SUCCEEDED(hr))
		{
			hr = m_pIWICFactory->CreateBitmapFromSource(
				pIDecoderFrame,          // Create a bitmap from the image frame
				WICBitmapCacheOnDemand,  // Cache metadata when needed
				&pIBitmap);              // Pointer to the bitmap
		}
		
		if (SUCCEEDED(hr))
		{
			hr = pIBitmap->Lock(0, WICBitmapLockWrite, &pILock);
			
			if (SUCCEEDED(hr))
			{
				UINT cbBufferSize = 0;
				BYTE *pv = NULL;

				if (SUCCEEDED(hr))
				{
					hr = pILock->GetDataPointer(&cbBufferSize, &pv);
				}
				m_buffer = new uint32_t[m_width * m_height];
				for (unsigned long long i = 0; i < cbBufferSize / 3; i++)
				{
					unsigned long long j = i * 3;
					m_buffer[i] = (pv[j]) + (pv[j + 1] << 8) + (pv[j + 2] << 16);
				}

				pILock->Release();
			}
		}

		pIDecoderFrame->Release();
		pIDecoder->Release();
	}

}