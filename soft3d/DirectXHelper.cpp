#include "soft3d.h"
#include "DirectXHelper.h"
#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")

using namespace Microsoft::WRL;

namespace soft3d {

	std::shared_ptr<DirectXHelper> DirectXHelper::s_instance(new DirectXHelper());

	DirectXHelper::DirectXHelper()
	{
		m_lastFps = 0;
		m_fps = 0;
		m_lastTickCount = 0;
	}


	DirectXHelper::~DirectXHelper()
	{
		ReleaseD2D();
	}


	void DirectXHelper::Init(HWND hwnd)
	{
		m_hWnd = hwnd;
		InitD2D();
	}


	void DirectXHelper::InitD2D()
	{
		D2D1_FACTORY_OPTIONS options;
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
		ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &m_d2dFactory));

		ComPtr<ID3D11Device> device;
		ComPtr<IDXGIDevice1> dxgiDevice;
		ComPtr<ID3D11DeviceContext> context;
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};
		ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, featureLevels,
			ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &m_featureLevel, &context));
		device.As(&dxgiDevice);
		ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));
		ThrowIfFailed(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dContext));

		// Allocate a descriptor.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = 0;                           // use automatic sizing
		swapChainDesc.Height = 0;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;                // don't use multi-sampling
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;                     // use double buffering to enable flip
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect
		swapChainDesc.Flags = 0;

		// Identify the physical adapter (GPU or card) this device is runs on.
		ComPtr<IDXGIAdapter> dxgiAdapter;
		ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

		// Get the factory object that created the DXGI device.
		ComPtr<IDXGIFactory2> dxgiFactory;
		ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

		// Get the final swap chain for this window from the DXGI factory.
		ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(device.Get(), m_hWnd, &swapChainDesc, nullptr, nullptr, &m_swapChain));

		// Ensure that DXGI doesn't queue more than one frame at a time.
		ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));

		// Get the backbuffer for this window which is be the final 3D render target.
		//ComPtr<ID3D11Texture2D> backBuffer;
		//ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

		// Now we set up the Direct2D render target bitmap linked to the swapchain. 
		// Whenever we render to this bitmap, it is directly rendered to the 
		// swap chain associated with the window.
		D2D1_PIXEL_FORMAT pixelFormat;

		pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
		D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		{
			pixelFormat,
			96.0f, 96.0f,
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			NULL
		};

		// Direct2D needs the dxgi version of the backbuffer surface pointer.
		ComPtr<IDXGISurface> dxgiBackBuffer;
		ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)));

		// Get a D2D surface from the DXGI back buffer to use as the D2D render target.
		ThrowIfFailed(m_d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &bitmapProperties, &m_d2dTargetBitmap));

		// Now we can set the Direct2D render target.
		m_d2dContext->SetTarget(m_d2dTargetBitmap);

		ThrowIfFailed(m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &m_pGreenBrush));

		IDWriteFactory* pWriteFactory;
		ThrowIfFailed(DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&pWriteFactory)
			));
		ThrowIfFailed(pWriteFactory->CreateTextFormat(
			L"Cambria",
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			20.0f,
			L"en-us",
			&m_pTextFormat
			));
		pWriteFactory->Release();
	}

	void DirectXHelper::ReleaseD2D()
	{
		m_pTextFormat->Release();
		m_d2dTargetBitmap->Release();
		m_pGreenBrush->Release();
		m_d2dContext->Release();
		m_d2dDevice->Release();
		m_d2dFactory->Release();
	}

	void DirectXHelper::Paint(const uint32* buffer, uint16 width, uint16 height)
	{
		if (m_d2dContext == NULL)
			return;

		D2D1_RECT_U rect = { 0, 0, width, height };
		ThrowIfFailed(m_d2dTargetBitmap->CopyFromMemory(&rect, buffer, width * 4));

		m_d2dContext->BeginDraw();

		m_fps++;
		if (GetTickCount() - m_lastTickCount > 1000)
		{
			m_lastFps = m_fps;
			m_fps = 0;
			m_lastTickCount = GetTickCount();
		}
		D2D1_RECT_F rectf = { 0.0f, 0.0f, (float)width, 0.0f };
		wchar_t buf[64];
		swprintf(buf, L"FPS:%d", m_lastFps);
		m_d2dContext->DrawTextW(buf, sizeof(wchar_t) * 3, m_pTextFormat, rectf, m_pGreenBrush);

		ThrowIfFailed(m_d2dContext->EndDraw());

		DXGI_PRESENT_PARAMETERS parameters;
		parameters.DirtyRectsCount = 0;
		parameters.pDirtyRects = NULL;
		parameters.pScrollOffset = NULL;
		parameters.pScrollRect = NULL;
		ThrowIfFailed(m_swapChain->Present1(1, 0, &parameters));
	}

}