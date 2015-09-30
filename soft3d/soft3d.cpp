// soft3d.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "soft3d.h"
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

using namespace Microsoft::WRL;

#define MAX_LOADSTRING 100

// 全局变量: 
HWND hWnd;
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

ID2D1Factory1* m_d2dFactory;
ID2D1Device* m_d2dDevice;
ID2D1DeviceContext* m_d2dContext;
D3D_FEATURE_LEVEL m_featureLevel;
IDXGISwapChain1* m_swapChain;
ComPtr<ID2D1Bitmap1> m_d2dTargetBitmap;

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw "Error";
	}
}

void InitD2D()
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
	ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(device.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &m_swapChain));

	// Ensure that DXGI doesn't queue more than one frame at a time.
	ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));

	// Get the backbuffer for this window which is be the final 3D render target.
	ComPtr<ID3D11Texture2D> backBuffer;
	ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

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
	m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
}

void ReleaseD2D()
{
	m_d2dContext->Release();
	m_d2dDevice->Release();
	m_d2dFactory->Release();
}

void Display()
{
	if (m_d2dContext == NULL)
		return;
	ComPtr<ID2D1SolidColorBrush> pBlackBrush;
	ThrowIfFailed(m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &pBlackBrush));

	m_d2dContext->BeginDraw();
	RECT rc;
	GetClientRect(hWnd, &rc);
	D2D1_RECT_F rectf = D2D1::RectF(rc.left + 100.0f, rc.top + 100.0f, rc.right - 100.0f, rc.bottom - 100.0f);

	m_d2dContext->DrawRectangle(rectf, pBlackBrush.Get());

	ThrowIfFailed(m_d2dContext->EndDraw());

	DXGI_PRESENT_PARAMETERS parameters;
	parameters.DirtyRectsCount = 0;
	parameters.pDirtyRects = NULL;
	parameters.pScrollOffset = NULL;
	parameters.pScrollRect = NULL;
	ThrowIfFailed(m_swapChain->Present1(1, 0, &parameters));
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SOFT3D, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SOFT3D));

    MSG msg;

	InitD2D();
	Display();

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	ReleaseD2D();

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SOFT3D));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SOFT3D);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
