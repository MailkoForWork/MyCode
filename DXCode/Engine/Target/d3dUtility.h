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

//回调函数类型
	typedef BOOL(*FInitAppFunc)();//程序运行最初，用于给用户设置程序相关信息
	typedef BOOL(*FRenderLoopFunc)(float delay);
	typedef BOOL(*FUpdateGameFunc)(float delay);//返回false将退出游戏
	typedef BOOL(*FSetUpSystemFunc)();//初始化渲染所需资源
	typedef BOOL(*FCleanUpSystemFunc)();//释放渲染所需资源

//GS函数
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

//程序启动变量
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


//函数代理
	WNDPROC m_WndProc;
	FInitAppFunc m_InitAppFunc;
	FRenderLoopFunc m_RenderLoopFunc;
	FUpdateGameFunc m_UpdateGameFunc;
	FSetUpSystemFunc m_SetUpSystemFunc;
	FCleanUpSystemFunc m_CleanUpSystemFunc;


};


