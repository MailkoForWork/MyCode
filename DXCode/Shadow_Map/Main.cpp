//=================================================
// CreateBy Mailko
// For 阴影深度贴图实现阴影效果
// EndTime 2016-11-26
//=================================================


#include "Target.h"

BOOL InitAppFunc();//程序运行最初，用于给用户设置程序相关信息
BOOL SetUpSystemFunc();//初始化渲染所需资源
BOOL RenderLoopFunc(float delay);
BOOL UpdateGameFunc(float delay);//返回false将退出游戏
BOOL CleanUpSystemFunc();//释放渲染所需资源

VOID InitCallFunction();

CRenderObject g_RenderObject;
CRenderObject g_ShadowMapObject;//上一个RenderObject的副本，用于渲染至Map的深度贴图
CRenderObject g_LightObject;
D3DXVECTOR3 g_LightPosition = D3DXVECTOR3(20, 20, 20);
D3DXVECTOR3 g_ObjectsCenter = D3DXVECTOR3(0, 0, 0);

CRenderTexture g_ShadowMap;
INT g_ShadowMapSize = 512;
FLOAT g_fNear = 10;
FLOAT g_fFar = 50;

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
	pCamera->setPosition(&D3DXVECTOR3(0, 10, -20));

	HRESULT hr = S_OK;

	// 装入模型
	TCHAR szAppPath[MAX_PATH];
	TCHAR szRcPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szAppPath);

	// 
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\NormalFile"));
	SetCurrentDirectory(szRcPath);
	TCHAR* szFile_1 = TEXT("ShmFile.shm");
	if (FAILED(hr = g_RenderObject.OnCreateDevice(pDevice, szFile_1))) return hr;

	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\LightFlag"));
	SetCurrentDirectory(szRcPath);
	if (FAILED(hr = g_LightObject.OnCreateDevice(pDevice, szFile_1))) return hr;

	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\NormalFileToRenderShadowMap"));
	SetCurrentDirectory(szRcPath);
	if (FAILED(hr = g_ShadowMapObject.OnCreateDevice(pDevice, szFile_1))) return hr;

	g_ShadowMap.OnCreateDevice(pDevice, g_ShadowMapSize, g_ShadowMapSize);

	//复原
	SetCurrentDirectory(szAppPath);
	return TRUE;
}

VOID RenderObjects()
{
	g_ShadowMapObject.OnFrameRender(CD3dUtility::GetInstance()->GetDevice());
}

BOOL RenderLoopFunc(float delay)
{
	HRESULT hr;
	
	CD3dUtility* d3d = CD3dUtility::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = d3d->GetDevice();

	//渲染阴影深度贴图
	g_ShadowMap.RenderTexture(pDevice, RenderObjects);
	if (false)
	{
		D3DXSaveTextureToFile(TEXT("D:\\Test.bmp"), D3DXIFF_BMP, g_ShadowMap.GetTexture(), NULL);
	}
	// 
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.25f, 0.25f, 0.55f), 1.0f, 0);

	// 渲染场景
	pDevice->BeginScene();
	{

		g_RenderObject.OnFrameRender(pDevice);
		g_LightObject.OnFrameRender(pDevice);

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
	D3DXMatrixPerspectiveFovLH(&matProject, D3DX_PI / 3, windowWidth / windowHeight, g_fNear, g_fFar);
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

	//绘制shadowMap的时候使用灯光位置作为视野位置
	D3DXMATRIXA16 matLightView;
	D3DXVECTOR3 viewUp = D3DXVECTOR3(0, 1, 0);
	D3DXMatrixLookAtLH(&matLightView, &g_LightPosition, &g_ObjectsCenter, &viewUp);
	D3DXMATRIXA16 matLightProj;
	D3DXMatrixPerspectiveFovLH(&matLightProj, D3DX_PI / 4, 1, g_fNear, g_fFar);
	g_ShadowMapObject.OnFrameMove(&matWorld, &matLightView, &matLightProj, 0);
	g_RenderObject.SetLightMatrix(matWorld, matLightView, matLightProj);//将以光照作为视点的变换矩阵传入，使得能在同一变换下比较深度
	g_RenderObject.SetShadowMap(g_ShadowMap.GetTexture());
	g_RenderObject.SetProjectDistance(g_fNear, g_fFar);
	//g_RenderObject.OnFrameMove(&matWorld, &matLightView, &matLightProj, 0);

	D3DXMATRIXA16 matLightWorld;
	D3DXMatrixTranslation(&matLightWorld, g_LightPosition.x, g_LightPosition.y, g_LightPosition.z);
	g_LightObject.OnFrameMove(&matLightWorld, &matView, &matProject, 0);
	return TRUE;
}

//释放渲染所需资源
BOOL CleanUpSystemFunc()
{
	return TRUE;
}