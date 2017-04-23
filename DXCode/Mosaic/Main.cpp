//=================================================
// CreateBy Mailko
// For 实现马赛克效果
// Tip 纹理是内存相关,数值不是
// EndTime 2016-11-26
//=================================================

#define RENDER_BY_INTERFACE 1
#include "Target.h"

BOOL InitAppFunc();//程序运行最初，用于给用户设置程序相关信息
BOOL SetUpSystemFunc();//初始化渲染所需资源
BOOL RenderLoopFunc(float delay);
BOOL UpdateGameFunc(float delay);//返回false将退出游戏
BOOL CleanUpSystemFunc();//释放渲染所需资源

VOID InitCallFunction();

CRenderObject g_RenderObject;
CBoard g_Board;
LPDIRECT3DTEXTURE9  g_RenderTexture;
LPDIRECT3DSURFACE9  g_RenderSurface;

INT WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPreInstance,
	PSTR cmdLine,
	INT showCmd)
{
	InitCallFunction();
	CD3dUtility* d3d = CD3dUtility::GetInstance();
	int result = d3d->Main(hInstance);
	SAFE_DELETE(d3d)
		return result;
}

VOID InitCallFunction()
{
	CD3dUtility* d3d = CD3dUtility::GetInstance();
	d3d->SetInitAppFunc(InitAppFunc);
	d3d->SetSetUpSystemFunc(SetUpSystemFunc);
	d3d->SetRenderLoopFunc(RenderLoopFunc);
	d3d->SetUpdateGameFunc(UpdateGameFunc);
	d3d->SetCleanUpSystemFunc(CleanUpSystemFunc);
	d3d->SetWindowSize(1024, 1024);
}

//程序运行最初，用于给用户设置程序相关信息
BOOL InitAppFunc()
{
	return TRUE;
}

//初始化渲染所需资源
BOOL SetUpSystemFunc()
{
	CD3dUtility* d3d = CD3dUtility::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = d3d->GetDevice();

	Camera* pCamera = d3d->GetCamera();
	pCamera->setPosition(&D3DXVECTOR3(0, 10, -30));

	//创建渲染纹理
	D3DXCreateTexture(pDevice, d3d->GetWindowWidth(), d3d->GetWindowHeight(), 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_RenderTexture);

	g_RenderTexture->GetSurfaceLevel(0, &g_RenderSurface);

	HRESULT hr = S_OK;

	// 装入模型
	TCHAR szAppPath[MAX_PATH];
	TCHAR szRcPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szAppPath);

	//原始模型
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\NormalFile"));
	SetCurrentDirectory(szRcPath);
	TCHAR* szFile_1 = TEXT("ShmFile.shm");
	if (FAILED(hr = g_RenderObject.OnCreateDevice(pDevice, szFile_1))) return hr;

	//马赛克处理
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\OriRenderTarget"));
	SetCurrentDirectory(szRcPath);
	HR(g_Board.OnCreateDevice(pDevice));
	g_Board.ChangeTexture(g_RenderTexture);
	g_Board.SetTextureSize(d3d->GetWindowWidth(), d3d->GetWindowHeight());
	g_Board.SetMosaicSize(8, 8);
	//复原
	SetCurrentDirectory(szAppPath);
	return TRUE;
}

BOOL RenderLoopFunc(float delay)
{
	HRESULT hr;

	CD3dUtility* d3d = CD3dUtility::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = d3d->GetDevice();
	// 
	//先将场景渲染到纹理上
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	pDevice->GetRenderTarget(0, &pBackBuffer);
	pDevice->SetRenderTarget(0, g_RenderSurface);

	// 渲染场景
	pDevice->BeginScene();
	{
		pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f), 1.0f, 0);

		g_RenderObject.OnFrameRender(pDevice);

		pDevice->EndScene();
	}
	pDevice->SetRenderTarget(0, pBackBuffer);
	if (false)
	{
		hr = D3DXSaveTextureToFile("D:\\Test.bmp", D3DXIFF_BMP, g_RenderTexture, NULL);
	}
	if (false)
	{
		hr = D3DXCreateTextureFromFile(pDevice, "D:\\Test.bmp", &g_RenderTexture);
	}

	//将渲染纹理绘制到颜色缓存，并进行马赛克处理
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
	pDevice->BeginScene();
	{

		g_Board.OnFrameRender(pDevice);

		pDevice->EndScene();
	}

	pDevice->Present(0, 0, 0, 0);
	return TRUE;
}

//返回false将退出游戏
BOOL UpdateGameFunc(float delay)
{
	CD3dUtility* d3d = CD3dUtility::GetInstance();
	Camera* pCamera = d3d->GetCamera();
	DirectInputClass* pInput = d3d->GetInput();
	pInput->GetInput();

	// 物体绕 Y 轴旋转
	static float time = 0;
	time += delay;
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMATRIXA16 matView;
	D3DXMATRIXA16 matProject;
	int windowHeight, windowWidth;
	CD3dUtility::GetInstance()->GetWindowSize(windowWidth, windowHeight);
	D3DXMatrixPerspectiveFovLH(&matProject, D3DX_PI / 4, windowWidth / windowHeight, 1.0, 100.0);
	static bool changeWorld = TRUE;
	if (pInput->IsKeyDownRight(DIK_SPACE))
	{
		changeWorld = !changeWorld;
	}
	if (changeWorld)
	{
		D3DXMatrixRotationY(&matWorld, time);
	}

	//照相机移动

	static const int  speed = 10;
	if(pInput->IsKeyDown(DIK_W))
	{ 
		pCamera->walk(delay * speed);
	}
	if (pInput->IsKeyDown(DIK_S))
	{
		pCamera->walk(-delay * speed);
	}
	if (pInput->IsKeyDown(DIK_D))
	{
		pCamera->strafe(delay * speed);
	}
	if (pInput->IsKeyDown(DIK_A))
	{
		pCamera->strafe(-delay * speed);
	}
	if (pInput->IsMouseButtonDown(0))
	{
		pCamera->yaw(delay * pInput->MouseDX());
		pCamera->pitch(delay * pInput->MouseDY());
	}

	pCamera->getViewMatrix(&matView);

	g_RenderObject.OnFrameMove(&matWorld, &matView, &matProject, 0);
	g_Board.OnFrameMove(0, D3DXVECTOR4(0, 0, 0, 1));
	return TRUE;
}

//释放渲染所需资源
BOOL CleanUpSystemFunc()
{
	return TRUE;
}