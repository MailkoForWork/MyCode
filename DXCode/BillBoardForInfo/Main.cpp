//=================================================
// CreateBy Mailko
// For �򵥹����ʵ��������۲����Ҵ�С����Զ���仯
// Tip �������������������任��������Ƶı�����λ�ò���������Щ�任��
//     ֻ����Ϊ���յĽ���������������.���������λ�ñ任�����˳������ˣ�Ҳ���Է�����ɫ����ʵ�֡�
//     Բ������������������Ϣ
// EndTime 2016-11-27
//=================================================


#include "Target.h"

BOOL InitAppFunc();//����������������ڸ��û����ó��������Ϣ
BOOL SetUpSystemFunc();//��ʼ����Ⱦ������Դ
BOOL RenderLoopFunc(float delay);
BOOL UpdateGameFunc(float delay);//����false���˳���Ϸ
BOOL CleanUpSystemFunc();//�ͷ���Ⱦ������Դ

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

	TCHAR szAppPath[MAX_PATH];
	TCHAR szRcPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szAppPath);

	// װ��ģ��:����ģ��
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\NormalFile"));
	SetCurrentDirectory(szRcPath);
	TCHAR* szFile_1 = TEXT("ShmFile.shm");
	if (FAILED(hr = g_RenderObject.OnCreateDevice(pDevice, szFile_1))) return hr;

	//װ��������
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\TeapotBillBoard"));
	SetCurrentDirectory(szRcPath);
	HR(g_TeapotBoard.OnCreateDevice(pDevice));

	//װ��Բ������
	lstrcpy(szRcPath, szAppPath);
	lstrcat(szRcPath, TEXT("\\TorusBillBoard"));
	SetCurrentDirectory(szRcPath);
	HR(g_TorusBoard.OnCreateDevice(pDevice));

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
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.55f), 1.0f, 0);

	// ��Ⱦ����
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

//����false���˳���Ϸ
BOOL UpdateGameFunc(float delay)
{
	static float time = 0;
	time += delay;

	CD3dUtility* d3d = CD3dUtility::GetInstance();
	Camera* pCamera = d3d->GetCamera();
	DirectInputClass* pInput = d3d->GetInput();
	pInput->GetInput();

	// ������ Y ����ת
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

//�ͷ���Ⱦ������Դ
BOOL CleanUpSystemFunc()
{
	g_RenderObject.OnDestroyDevice();
	g_TeapotBoard.OnDestroyDevice();
	g_TorusBoard.OnDestroyDevice();
	return TRUE;
}