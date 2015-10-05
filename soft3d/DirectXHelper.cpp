#include "stdafx.h"
#include "soft3d.h"
#include "DirectXHelper.h"
#include "Soft3dPipeline.h"
#include "SceneManager.h"

using namespace Microsoft::WRL;

namespace soft3d {

	std::shared_ptr<DirectXHelper> DirectXHelper::s_instance(new DirectXHelper());

	DirectXHelper::DirectXHelper()
	{
	}


	DirectXHelper::~DirectXHelper()
	{
		delete[] m_operateBuffer;
		ReleaseD2D();
	}


	void DirectXHelper::Init(HWND hwnd, uint16 width, uint16 height)
	{
		m_hWnd = hwnd;
		m_width = width;
		m_height = height;
		m_operateBuffer = new unsigned int[m_width * m_height];
		InitD2D();

		SceneManager::Instance()->Init();
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
	}

	void DirectXHelper::ReleaseD2D()
	{
		m_d2dContext->Release();
		m_d2dDevice->Release();
		m_d2dFactory->Release();
		m_d2dTargetBitmap->Release();
	}

	void DirectXHelper::Display()
	{
		if (m_d2dContext == NULL)
			return;

		m_d2dContext->BeginDraw();

		Clear(0);
		Soft3dPipeline::Instance()->Process();
		D2D1_RECT_U rect = { 0, 0, m_width, m_height };
		ThrowIfFailed(m_d2dTargetBitmap->CopyFromMemory(&rect, m_operateBuffer, m_width * 4));

		ThrowIfFailed(m_d2dContext->EndDraw());

		DXGI_PRESENT_PARAMETERS parameters;
		parameters.DirtyRectsCount = 0;
		parameters.pDirtyRects = NULL;
		parameters.pScrollOffset = NULL;
		parameters.pScrollRect = NULL;
		ThrowIfFailed(m_swapChain->Present1(1, 0, &parameters));
	}

	int DirectXHelper::DrawPixel(uint16 x, uint16 y, uint32 color, uint16 size)
	{
		if (x > m_width || y > m_height)
			return -1;
		if (size > 100 || size < 1)
			size = 1;

		for (uint16 i = 0; i < size; i++)
		{
			for (uint16 j = 0; j < size; j++)
				SetOperateBuffer((y+j - size/2) * m_width + x+i - size/2, color);
		}
		return 0;
	}

	void DirectXHelper::SetOperateBuffer(uint32 index, uint32 value)
	{
		if (index >= (uint32)m_width * m_height)
			return;
		m_operateBuffer[index] = value;
	}

	int DirectXHelper::Clear(uint32 color)
	{
		memset(m_operateBuffer, color, m_width * m_height * sizeof(uint32));
		return 0;
	}

}