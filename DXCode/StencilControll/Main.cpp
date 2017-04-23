//=================================================
// CreateBy Mailko
// For 模板缓存控制渲染写入
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
CBoard g_StencilArea;

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

	// 装入模型
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\NormalFile"));
	SetCurrentDirectory(szRcPath);
	TCHAR* szFile_1 = TEXT("ShmFile.shm");
	if (FAILED(hr = g_RenderObject.OnCreateDevice(pDevice, szFile_1))) return hr;

	//加载模板范围
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\StencilArea"));
	SetCurrentDirectory(szRcPath);
	HR(g_StencilArea.OnCreateDevice(pDevice));
	

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
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER |D3DCLEAR_STENCIL, D3DXCOLOR(0.0f, 0.25f, 0.25f, 0.55f), 1.0f, 0);

	//模板值写入
	//关闭深度缓存和颜色缓存的写入
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//通过设置融合因子来阻止颜色缓存的写入
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//开启模板缓存
	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_STENCILREF, 0x01);
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	pDevice->BeginScene();
	{
		g_StencilArea.OnFrameRender(pDevice);
		pDevice->EndScene();
	}

	//恢复颜色缓存和深度缓存的读写
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	//模板缓存开启限制
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL); //此时的模板缓存写入值为1
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	// 渲染场景
	pDevice->BeginScene();
	{

		g_RenderObject.OnFrameRender(pDevice);
		g_StencilArea.OnFrameRender(pDevice);
		pDevice->EndScene();
	}

	//回到最初状态
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
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
	return TRUE;
}

//释放渲染所需资源
BOOL CleanUpSystemFunc()
{
	return TRUE;
}