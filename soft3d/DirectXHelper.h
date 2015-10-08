#pragma once
#include <d2d1.h>
#include <d2d1_1.h>
#include <d3d.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <wrl.h>
#include <exception>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#include <memory>

namespace soft3d
{

	class DirectXHelper
	{
	public:
		static std::shared_ptr<DirectXHelper> Instance() {
			return s_instance;
		}
		~DirectXHelper();

		void Init(HWND hwnd);
		void Paint(const uint32* buffer, uint16 width, uint16 height);

	protected:
		DirectXHelper();
		void InitD2D();
		void ReleaseD2D();

	private:
		inline void ThrowIfFailed(HRESULT hr) {
			if (FAILED(hr))
			{
				throw "Error";
			}
		}


	private:
		HWND m_hWnd;

		ID2D1Factory1* m_d2dFactory;
		ID2D1Device* m_d2dDevice;
		ID2D1DeviceContext* m_d2dContext;
		D3D_FEATURE_LEVEL m_featureLevel;
		IDXGISwapChain1* m_swapChain;
		ID2D1Bitmap1* m_d2dTargetBitmap;

		ID2D1SolidColorBrush* m_pGreenBrush;
		IDWriteTextFormat* m_pTextFormat;

		DWORD m_lastTickCount;
		uint16 m_fps;
		uint16 m_lastFps;

		static std::shared_ptr<DirectXHelper> s_instance;
	};

}

