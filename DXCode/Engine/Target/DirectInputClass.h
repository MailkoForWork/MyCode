//**********************************************//
// Name:Direct3DInputClass.h                    //
// Desc:获取设备输入                            //
// Author:mailko                                //
// Date:2015.04.26                              //
//**********************************************//
#ifndef _DIRECTINPUTCLASS_H_
#define _DIRECTINPUTCLASS_H_
#include<dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#include"Common.h"
//输入类声明：
class DLL_API DirectInputClass{
//成员变量声明：
private:
	IDirectInput8 *pDI8;
	IDirectInputDevice8 *pMouseDI;
	IDirectInputDevice8 *pKeyboardDI;
	char keyboardState[256];
	char lastKeyboardState[256];//上次的键盘状态
	DIMOUSESTATE mouseState;
	DIMOUSESTATE lastMouseState;//上次的鼠标状态
//成员函数声明：
public:
	DirectInputClass();//构造函数，只负责成员变量的初始化
	~DirectInputClass();//析构函数，负责成员变量的释放
	HRESULT DirectInputInit(HWND hwnd,HINSTANCE hInstance);//获取输入设备接口
	HRESULT GetInput();//获取当前输入，重置输入状态
	BOOL IsKeyDown(UINT key);//判断某键是否按下
	BOOL IsMouseButtonDown(UINT button);//判断鼠标某键是否按下
	BOOL IsKeyDownRight(UINT key);
	BOOL IsKeyUpRight(UINT key);//判断某键是否刚被弹起
	BOOL IsMouseButtonUpRight(UINT button);//判断鼠标某键是否刚被释放
	BOOL IsMouseButtonDownRight(UINT button);
	//返回鼠标个方向的移动距离
	float MouseDX();
	float MouseDY();
	float MouseDZ();
};



#endif