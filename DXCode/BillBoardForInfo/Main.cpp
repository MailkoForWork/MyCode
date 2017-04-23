//=================================================
// CreateBy Mailko
// For 简单广告牌实例，朝向观察者且大小不随远近变化
// Tip 广告牌中心随世界坐标变换，但广告牌的本身顶点位置并不参与这些变换，
//     只是作为最终的结果添加上中心坐标.中心坐标的位置变换放在了程序中了，也可以放在着色器中实现。
//     圆环广告板则加入了深度信息
// EndTime 2016-11-27
//=================================================


#include "Target.h"

BOOL InitAppFunc();//程序运行最初，用于给用户设置程序相关信息
BOOL SetUpSystemFunc();//初始化渲染所需资源
BOOL RenderLoopFunc(float delay);
BOOL UpdateGameFunc(float delay);//返回false将退出游戏
BOOL CleanUpSystemFunc();//释放渲染所需资源

VOID InitCallFunction();

CRenderObject g_RenderObject;
CBoard g_TeapotBoard;
CBoard g_TorusBoard;

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
	pCamera->setPosition(&D3DXVECTOR3(0, 10, -50));

	HRESULT hr = S_OK;

	TCHAR szAppPath[MAX_PATH];
	TCHAR szRcPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szAppPath);

	// 装入模型:基础模型
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\NormalFile"));
	SetCurrentDirectory(szRcPath);
	TCHAR* szFile_1 = TEXT("ShmFile.shm");
	if (FAILED(hr = g_RenderObject.OnCreateDevice(pDevice, szFile_1))) return hr;

	//装入茶壶广告版
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\TeapotBillBoard"));
	SetCurrentDirectory(szRcPath);
	HR(g_TeapotBoard.OnCreateDevice(pDevice));

	//装入圆环广告版
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\TorusBillBoard"));
	SetCurrentDirectory(szRcPath);
	HR(g_TorusBoard.OnCreateDevice(pDevice));

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
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.55f), 1.0f, 0);

	// 渲染场景
	pDevice->BeginScene();
	{

		g_RenderObject.OnFrameRender(pDevice);
		g_TeapotBoard.OnFrameRender(pDevice);
		g_TorusBoard.OnFrameRender(pDevice);

		pDevice->EndScene();
	}
	pDevice->Present(0, 0, 0, 0);
	return TRUE;
}

//返回false将退出游戏
BOOL UpdateGameFunc(float delay)
{
	static float time = 0;
	time += delay;

	CD3dUtility* d3d = CD3dUtility::GetInstance();
	Camera* pCamera = d3d->GetCamera();
	DirectInputClass* pInput = d3d->GetInput();
	pInput->GetInput();

	// 物体绕 Y 轴旋转
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
	D3DXVECTOR4 v4Position = D3DXVECTOR4(5, 6, -5, 1);
	//D3DXVECTOR4 v4Position = D3DXVECTOR4(0, 0, 0, 1);
	g_TeapotBoard.OnFrameMove(time, v4Position);	
	g_TeapotBoard.SetTranslateMatrix(&matWorld, &matView, &matProject);

	D3DXVECTOR4 v4TorusPosition = D3DXVECTOR4(-5, 6, 5, 1);
	//D3DXVECTOR4 v4Position = D3DXVECTOR4(0, 0, 0, 1);
	g_TorusBoard.OnFrameMove(time, v4TorusPosition);
	g_TorusBoard.SetTranslateMatrix(&matWorld, &matView, &matProject);
	return TRUE;
}

//释放渲染所需资源
BOOL CleanUpSystemFunc()
{
	g_RenderObject.OnDestroyDevice();
	g_TeapotBoard.OnDestroyDevice();
	g_TorusBoard.OnDestroyDevice();
	return TRUE;
}