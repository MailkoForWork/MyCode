//=================================================
// CreateBy Mailko
// For ����ģ��
// EndTime 2016-11-26
//=================================================


#include "Target.h"

BOOL InitAppFunc();//����������������ڸ��û����ó��������Ϣ
BOOL SetUpSystemFunc();//��ʼ����Ⱦ������Դ
BOOL RenderLoopFunc(float delay);
BOOL UpdateGameFunc(float delay);//����false���˳���Ϸ
BOOL CleanUpSystemFunc();//�ͷ���Ⱦ������Դ

VOID InitCallFunction();

CRenderObject g_RenderObject;
CRenderObject g_Cube;
CShadowVolumeMesh g_CubeShadowVolume;
CRenderObject g_LightObject;
CBoard g_ShadowMap;

D3DXVECTOR3 g_LightPosition = D3DXVECTOR3(10, 10, 10);

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

//����������������ڸ��û����ó��������Ϣ
BOOL InitAppFunc()
{
	return TRUE;
}

//��ʼ����Ⱦ������Դ
BOOL SetUpSystemFunc()
{
	CD3dUtility* d3d = CD3dUtility::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = d3d->GetDevice();

	Camera* pCamera = d3d->GetCamera();
	pCamera->setPosition(&D3DXVECTOR3(0, 10, -50));

	HRESULT hr = S_OK;

	// װ��ģ��
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
	lstrcat(szRcPath, TEXT("\\Cube"));
	SetCurrentDirectory(szRcPath);
	if (FAILED(hr = g_Cube.OnCreateDevice(pDevice, szFile_1))) return hr;

	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\CubeShadowVolume"));
	SetCurrentDirectory(szRcPath);
	if (FAILED(hr = g_CubeShadowVolume.OnDeviceCreate(g_Cube.GetMesh(), pDevice))) return hr;

	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\LightFlag"));
	SetCurrentDirectory(szRcPath);
	if (FAILED(hr = g_LightObject.OnCreateDevice(pDevice, szFile_1))) return hr;

	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\ShadowMap"));
	SetCurrentDirectory(szRcPath);
	if (FAILED(hr = g_ShadowMap.OnCreateDevice(pDevice))) return hr;
	//��ԭ
	SetCurrentDirectory(szAppPath);
	return TRUE;
}

BOOL RenderLoopFunc(float delay)
{
	HRESULT hr;
	
	CD3dUtility* d3d = CD3dUtility::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = d3d->GetDevice();
	// 
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.0f, 0.25f, 0.25f, 0.55f), 1.0f, 0);

	// �Ȼ���һ����Ӱ�����壬��ȡ���ͼ
	pDevice->BeginScene();
	{
		g_RenderObject.OnFrameRender(pDevice);
		g_Cube.OnFrameRender(pDevice);
		g_LightObject.OnFrameRender(pDevice);
		g_CubeShadowVolume.OnFrameRender(pDevice);
		pDevice->EndScene();
	}

	//���Ƶڶ������ģ�建�����ã���ʱ��������ɫ����
	pDevice->BeginScene();
	{
		g_CubeShadowVolume.OnFrameRender(pDevice);
		pDevice->EndScene();
	}

	//���Ƶ����������Ӱ�����ں�
	pDevice->BeginScene();
	{
		//g_Cube.OnFrameRender(pDevice);
		//����һ������Ӱͼ��
		g_ShadowMap.OnFrameRender(pDevice);
		pDevice->EndScene();
	}

	pDevice->Present(0, 0, 0, 0);
	return TRUE;
}

//����false���˳���Ϸ
BOOL UpdateGameFunc(float delay)
{
	CD3dUtility* d3d = CD3dUtility::GetInstance();
	Camera* pCamera = d3d->GetCamera();
	DirectInputClass* pInput = d3d->GetInput();
	pInput->GetInput();

	// ������ Y ����ת
	static float time = 0;
	time += delay;
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMATRIXA16 matView;
	D3DXMATRIXA16 matProject;
	int windowHeight, windowWidth;
	CD3dUtility::GetInstance()->GetWindowSize(windowWidth, windowHeight);
	D3DXMatrixPerspectiveFovLH(&matProject, D3DX_PI / 4, windowWidth / windowHeight, 0.1, 1000.0);
	static bool changeWorld = TRUE;
	if (pInput->IsKeyDownRight(DIK_SPACE))
	{
		changeWorld = !changeWorld;
	}
	if (changeWorld)
	{
		D3DXMatrixRotationY(&matWorld, time / 10);
	}

	//������ƶ�

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
	D3DXMATRIXA16 matCubeWorld;
	D3DXMatrixTranslation(&matCubeWorld, 0, 5, 0);
	matCubeWorld = matCubeWorld * matWorld;
	g_Cube.OnFrameMove(&matCubeWorld, &matView, &matProject, 0);
	g_CubeShadowVolume.OnFrameMove(&g_LightPosition, &matCubeWorld, &matView, &matProject, 0);

	D3DXMATRIXA16 matLightWorld;
	D3DXMatrixTranslation(&matLightWorld, g_LightPosition.x, g_LightPosition.y, g_LightPosition.z);
	g_LightObject.OnFrameMove(&matLightWorld, &matView, &matProject, 0);

	g_ShadowMap.OnFrameMove(time, D3DXVECTOR4(0, 0, 0, 1));
	return TRUE;
}

//�ͷ���Ⱦ������Դ
BOOL CleanUpSystemFunc()
{
	g_RenderObject.OnDestroyDevice();
	return TRUE;
}