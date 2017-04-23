//=================================================
// CreateBy Mailko
// For CuTextureʵ��С����Ч��
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
CRenderObjectCube g_SkyObject;
CRenderCubeTexture g_RenderCubeTexture;
CRenderObjectCube g_ReflectionSphere;

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
	pCamera->setPosition(&D3DXVECTOR3(0, 10, -30));

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
	lstrcat(szRcPath, TEXT("\\SkyBox"));
	SetCurrentDirectory(szRcPath);
	HR(g_SkyObject.OnCreateDevice(pDevice, szFile_1));

	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\ReflectionSphere"));
	SetCurrentDirectory(szRcPath);
	HR(g_ReflectionSphere.OnCreateDevice(pDevice, szFile_1));

	//��ԭ
	SetCurrentDirectory(szAppPath);

	HR(g_RenderCubeTexture.OnCreateDevice(pDevice));

	return TRUE;
}

VOID RenderObjects(const D3DXMATRIX& matView)
{
	CD3dUtility* d3d = CD3dUtility::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = d3d->GetDevice();
	g_RenderObject.SetViewMatrix(matView);
	g_SkyObject.SetViewMatrix(matView);
	g_RenderObject.OnFrameRender(pDevice);
	g_SkyObject.OnFrameRender(pDevice);
}
BOOL RenderLoopFunc(float delay)
{
	HRESULT hr;
	
	CD3dUtility* d3d = CD3dUtility::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = d3d->GetDevice();

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0);

	// ��Ⱦ����
	pDevice->BeginScene();
	{
		g_RenderObject.OnFrameRender(pDevice);
		g_SkyObject.OnFrameRender(pDevice);
		g_ReflectionSphere.OnFrameRender(pDevice);

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
	D3DXMatrixPerspectiveFovLH(&matProject, D3DX_PI / 2, windowWidth / windowHeight, 1.0, 100.0); //�����������ӽǱ���90��
	static bool changeWorld = TRUE;
	if (pInput->IsKeyDownRight(DIK_SPACE))
	{
		changeWorld = !changeWorld;
	}
	if (changeWorld)
	{
		D3DXMatrixRotationY(&matWorld, time);
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

	//��ͷģ�����������ƶ�
	D3DXMATRIX matObjectPos;
	D3DXMatrixTranslation(&matObjectPos, 0, -5, 0);
	matWorld = matObjectPos * matWorld;

	g_RenderObject.OnFrameMove(&matWorld, &matView, &matProject, 0);
	D3DXMATRIXA16 matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	g_SkyObject.OnFrameMove(&matIdentity, &matView, &matProject, 0);
	D3DXMATRIX matWorldSphere;
	D3DXMatrixTranslation(&matWorldSphere, 0, 20.0, 0);
	g_ReflectionSphere.OnFrameMove(&matIdentity, &matView, &matProject, 0);

	//��������������,����������Ϊ�˲��ƻ�֮��Զ������õ�Object;
	g_RenderCubeTexture.RenderCube(d3d->GetDevice(), RenderObjects);
	g_ReflectionSphere.SetTexture(g_RenderCubeTexture.GetTexture());
	
	g_SkyObject.SetViewMatrix(matView);
	g_RenderObject.SetViewMatrix(matView);
	return TRUE;
}

//�ͷ���Ⱦ������Դ
BOOL CleanUpSystemFunc()
{
	g_SkyObject.OnDestroyDevice();
	g_RenderObject.OnDestroyDevice();
	return TRUE;
}