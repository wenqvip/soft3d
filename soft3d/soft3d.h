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

namespace soft3d {

typedef unsigned short uint16;
typedef unsigned int uint32;

class soft3dEngine
{
public:
	soft3dEngine(HWND hwnd, uint16 width, uint16 height);
	~soft3dEngine();

	void Display();
	int DrawPixel(uint16 x, uint16 y, uint32 value, uint16 size);

protected:
	void InitD2D();
	void ReleaseD2D();

private:
	inline void ThrowIfFailed(HRESULT hr) {
		if (FAILED(hr))
		{
			throw "Error";
		}
	}

	void SetOperateBuffer(uint32 index, uint32 value);

private:
	HWND m_hWnd;
	uint16 m_width;
	uint16 m_height;

	ID2D1Factory1* m_d2dFactory;
	ID2D1Device* m_d2dDevice;
	ID2D1DeviceContext* m_d2dContext;
	D3D_FEATURE_LEVEL m_featureLevel;
	IDXGISwapChain1* m_swapChain;
	ID2D1Bitmap1* m_d2dTargetBitmap;

	unsigned int* m_operateBuffer;
};

}

