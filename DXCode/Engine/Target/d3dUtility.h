#pragma once
//CreateTime 2013/10/29
//Author     mailko
//CreateFor  DX

#include<d3d9.h>
#include<limits>
#include"Common.h"
#include"Camera.h"
#include"DirectInputClass.h"
class DLL_API CD3dUtility
{
public:
	INT Main(HINSTANCE hInstance);
	bool InitD3D();
	static CD3dUtility* GetInstance();
	~CD3dUtility();

	void GetWindowSize(int &width, int &height);
	void SetWindowSize(const int &width, const int& height);
	void SetWindowName(const STRING& name);

	
	IDirect3DDevice9* GetDevice(){return m_Device;} 

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int EnterMsgLoop();

//�ص���������
	typedef BOOL(*FInitAppFunc)();//����������������ڸ��û����ó��������Ϣ
	typedef BOOL(*FRenderLoopFunc)(float delay);
	typedef BOOL(*FUpdateGameFunc)(float delay);//����false���˳���Ϸ
	typedef BOOL(*FSetUpSystemFunc)();//��ʼ����Ⱦ������Դ
	typedef BOOL(*FCleanUpSystemFunc)();//�ͷ���Ⱦ������Դ

//GS����
	SET_FUNCTION(FRenderLoopFunc, RenderLoopFunc)
	SET_FUNCTION(FUpdateGameFunc, UpdateGameFunc)
	SET_FUNCTION(FSetUpSystemFunc, SetUpSystemFunc)
	SET_FUNCTION(FCleanUpSystemFunc, CleanUpSystemFunc)
	SET_FUNCTION(FInitAppFunc, InitAppFunc)
	GET_FUNCTION(HINSTANCE, HInstance)
	GET_FUNCTION(HWND, HWnd)
	GET_FUNCTION(Camera*, Camera)
	GET_FUNCTION(DirectInputClass*, Input)
	GET_FUNCTION(INT, WindowWidth)
	GET_FUNCTION(INT, WindowHeight)

private:
	CD3dUtility();

//������������
	STRING m_WindowName;
	static CD3dUtility* m_Instance;
	HINSTANCE m_HInstance;
	INT m_WindowWidth;
	INT m_WindowHeight;
	D3DDEVTYPE m_DeviceType; //HAL or REF
	IDirect3DDevice9* m_Device;
	HWND m_HWnd;
	Camera* m_Camera;
	DirectInputClass* m_Input;


//��������
	WNDPROC m_WndProc;
	FInitAppFunc m_InitAppFunc;
	FRenderLoopFunc m_RenderLoopFunc;
	FUpdateGameFunc m_UpdateGameFunc;
	FSetUpSystemFunc m_SetUpSystemFunc;
	FCleanUpSystemFunc m_CleanUpSystemFunc;


};


