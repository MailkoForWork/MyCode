#define DLL_IMPLEMENT

#include"d3dUtility.h"
#pragma comment(lib,"winmm.lib")

CD3dUtility* CD3dUtility::m_Instance = NULL;
CD3dUtility* CD3dUtility::GetInstance()
{
	if (m_Instance == NULL)
	{
		m_Instance = new CD3dUtility[1];
	}
	return m_Instance;
}

INT CD3dUtility::Main(HINSTANCE hInstance)
{
	m_HInstance = hInstance;
	if (
		(m_InitAppFunc == NULL || m_InitAppFunc())
		&& InitD3D()
		&& m_SetUpSystemFunc == NULL || m_SetUpSystemFunc()
		)
	{
		EnterMsgLoop();
		m_CleanUpSystemFunc && m_CleanUpSystemFunc();
		return 1;
	}
	else
	{
		return 0;
	}
}

CD3dUtility::CD3dUtility()
	: m_HInstance(NULL)
	, m_WindowHeight(600)
	, m_WindowWidth(800)
	, m_Device(0)
	, m_DeviceType(D3DDEVTYPE_HAL)
	, m_HWnd(NULL)
	, m_RenderLoopFunc(NULL)
	, m_UpdateGameFunc(NULL)
	, m_SetUpSystemFunc(NULL)
	, m_CleanUpSystemFunc(NULL)
	, m_InitAppFunc(NULL)
	, m_Camera(NULL)
	, m_Input(NULL)
{
	m_WindowName = "DefaultApp";
	m_WndProc = WNDPROC(WndProc);
}

CD3dUtility::~CD3dUtility()
{
	SAFE_RELEASE(m_Device);
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Input);
}

void CD3dUtility::SetWindowSize(const int &width, const int& height)
{
	if (m_Device)
	{
		LOG("Warning:暂不支持初始化后重定义窗口大小")
	}
	else
	{
		m_WindowWidth = width;
		m_WindowHeight = height;
	}
}
void CD3dUtility::GetWindowSize(int &width, int &height)
{
	width = m_WindowHeight;
	height = m_WindowHeight;
}

void CD3dUtility::SetWindowName(const STRING& name)
{
	if (m_HWnd)
	{
		SetWindowText(m_HWnd, name.c_str());
	}
	m_WindowName = name;
}
bool CD3dUtility::InitD3D()
{	
	//{{InitWindow
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = m_WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_HInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = m_WindowName.c_str();

	if (!RegisterClass(&wc))
	{
		::MessageBox(0, "REgisterClass() -FAILED", 0, 0);
		return FALSE;
	}
	m_HWnd = CreateWindow(m_WindowName.c_str(), m_WindowName.c_str(), WS_POPUP | WS_CAPTION,
		0, 0, m_WindowWidth, m_WindowHeight,
		0, 0, m_HInstance, 0);

	if (!m_HWnd)
	{
		::MessageBox(0, "CreeateWindow() -FAILED", 0, 0);
		return FALSE;
	}

	::ShowWindow(m_HWnd, SW_SHOW);
	::UpdateWindow(m_HWnd);
	//InitWindow}}

	//{{Init D3D
	HRESULT hr = 0;
	IDirect3D9* d3d9 = NULL;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d9)
	{
		::MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
		return FALSE;
	}

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, m_DeviceType, &caps);

	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = m_WindowWidth;
	d3dpp.BackBufferHeight = m_WindowHeight;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = m_HWnd;
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		m_DeviceType,
		m_HWnd,
		vp, 
		&d3dpp,
		&m_Device
		);
	d3d9->Release(); 
	if (FAILED(hr))
	{
		::MessageBox(0, "CreateDevice() - FAILED", 0, 0);
		return FALSE;
	}
	//Init D3D}}

	//{{Init Camera
	m_Camera = new Camera[1];
	//InitCamera}}

	//{{Init Input
	m_Input = new DirectInputClass[1];
	m_Input->DirectInputInit(m_HWnd, m_HInstance);

	return TRUE;
}

int CD3dUtility::EnterMsgLoop()
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;
			if (m_UpdateGameFunc && !m_UpdateGameFunc(timeDelta))
			{
				break;
			}
			m_RenderLoopFunc && m_RenderLoopFunc(timeDelta);
			lastTime = currTime;
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK CD3dUtility::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}