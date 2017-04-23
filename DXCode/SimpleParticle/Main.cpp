//=================================================
// CreateBy Mailko
// For 基本模板
// EndTime 2016-11-26
//=================================================


#include "Target.h"
#include "Inc/SimpleParticle.h"

#define PARTICLE_COUNT 250


BOOL InitAppFunc();//程序运行最初，用于给用户设置程序相关信息
BOOL SetUpSystemFunc();//初始化渲染所需资源
BOOL RenderLoopFunc(float delay);
BOOL UpdateGameFunc(float delay);//返回false将退出游戏
BOOL CleanUpSystemFunc();//释放渲染所需资源

VOID InitCallFunction();

CRenderObject g_RenderObject;
CSimpleParticleSystem g_ParticleSystem;
LPD3DXEFFECT g_pParticleEffect;

INT WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPreInstance,
	PSTR cmdLine,
	INT showCmd)
{
	D3DXVECTOR3 axeX, axeY, axeZ;
	D3DXMATRIX Matrix;
	D3DXMatrixRotationYawPitchRoll(&Matrix, Rotation.y, Rotation.x, Rotation.z);
	axeX = D3DXVECTOR3(1, 0, 0);
	D3DXVec3TransformCoord(&axeX, &axeX, &Matrix);
	axeY = D3DXVECTOR3(1, 0, 0);
	D3DXVec3TransformCoord(&axeY, &axeY, &Matrix);
	axeZ = D3DXVECTOR3(1, 0, 0);
	D3DXVec3TransformCoord(&axeZ, &axeZ, &Matrix);
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

	//粒子系统：
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\ParticleFile"));
	SetCurrentDirectory(szRcPath);
	g_ParticleSystem.OnCreateDevice(pDevice, PARTICLE_COUNT);

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
		g_ParticleSystem.OnFrameRender(pDevice);

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
	STSimpleParticleParam pEffectParam;
	pEffectParam.pWorld = &matWorld;
	pEffectParam.pView = &matView;
	pEffectParam.pProject = &matProject;
	g_ParticleSystem.OnFrameMove(delay, (void*)&pEffectParam);

	return TRUE;
}

//释放渲染所需资源
BOOL CleanUpSystemFunc()
{
	g_RenderObject.OnDestroyDevice();
	return TRUE;
}