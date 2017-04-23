//***************************************************************//
//     Name:DirectInputClass                                     //
//     Desc:设备输入接口类，捕获键盘和鼠标的当前状态             //
//***************************************************************//
#define DLL_IMPLEMENT
#include"DirectInputClass.h"
//-------------------------------------------------
// Name:DirectInputClass：：构造函数
// Desc: 变量的基本初始化
//-------------------------------------------------

DirectInputClass::DirectInputClass()
{
	ZeroMemory(keyboardState,sizeof(char)*256);
	ZeroMemory(lastKeyboardState,sizeof(char)*256);
	ZeroMemory(&mouseState,sizeof(mouseState));
	ZeroMemory(&lastMouseState,sizeof(mouseState));
	pDI8=NULL;
	pKeyboardDI=NULL;
	pMouseDI=NULL;
}

//--------------------------------------------------
//Name:DirectInputClass::~DirectInputClass()
//Desc:析构函数
//--------------------------------------------------
DirectInputClass::~DirectInputClass()
{
	SAFE_RELEASE(pMouseDI);
	SAFE_RELEASE(pKeyboardDI);
	SAFE_RELEASE(pDI8);
}

//--------------------------------------------------
//  Name:DirectInputClass::DirectInputInit
//  Desc:获取鼠标设备和键盘设备
//--------------------------------------------------

HRESULT DirectInputClass::DirectInputInit(HWND hwnd,HINSTANCE hInstance)
{
	HRESULT hr;
	HR(DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,
		(void**)&pDI8,NULL));
	//获取键盘设备：
	HR(pDI8->CreateDevice(GUID_SysKeyboard,&pKeyboardDI,NULL));
	HR(pKeyboardDI->SetDataFormat(&c_dfDIKeyboard));
	HR(pKeyboardDI->SetCooperativeLevel(hwnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE));
	//HR(pKeyboardDI->Acquire());
	//HR(pKeyboardDI->Poll());
	//获取鼠标设备：
	HR(pDI8->CreateDevice(GUID_SysMouse,&pMouseDI,NULL));
	HR(pMouseDI->SetCooperativeLevel(hwnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE));
	HR(pMouseDI->SetDataFormat(&c_dfDIMouse));
	//HR(pMouseDI->Acquire());
	//HR(pMouseDI->Poll());
	return S_OK;
}


//------------------------------------------------
// Name:DirectInputClass::GetInput()
// Desc:获取输入设备当前状态
//-----------------------------------------------
HRESULT DirectInputClass::GetInput()
{
	memcpy(lastKeyboardState,keyboardState,sizeof(char)*256);
	memcpy(&lastMouseState,&mouseState,sizeof(mouseState));
	ZeroMemory(keyboardState,sizeof(char)*256);
	ZeroMemory(&mouseState,sizeof(mouseState));
	HRESULT hr=pKeyboardDI->GetDeviceState(sizeof(char)*256,(VOID**)&keyboardState);
	if(hr)
	{
		pKeyboardDI->Acquire();
		pKeyboardDI->GetDeviceState(sizeof(char)*256,(VOID**)&keyboardState);
	}
	hr=pMouseDI->GetDeviceState(sizeof(DIMOUSESTATE),(VOID**)&mouseState);
	if(hr)
	{
		pMouseDI->Acquire();
		pKeyboardDI->GetDeviceState(sizeof(DIMOUSESTATE),(VOID**)&mouseState);
	}
	return S_OK;
}


//-------------------------------------------------------------------
//  Name：DirectInputClass：：IsKeyDown()
//  Desc: 判断该按键是否处于按下状态
//-------------------------------------------------------------------

BOOL DirectInputClass::IsKeyDown(UINT key)
{
	if(keyboardState[key]&0x80)
		return true;
	return FALSE;
}
BOOL DirectInputClass::IsMouseButtonDown(UINT button)
{
	if(mouseState.rgbButtons[button]&0x80)
		return true;
	return FALSE;
}

//----------------------------------------------------------------
// Name:DirectInputClass：：MouseDX,MouseDY,MouseDX
// Desc:获取鼠标的位置变化
//-----------------------------------------------------------------

FLOAT DirectInputClass::MouseDX()
{
	return (FLOAT)mouseState.lX;
}
FLOAT DirectInputClass::MouseDY()
{
	return (FLOAT)mouseState.lY;
}
FLOAT DirectInputClass::MouseDZ()
{
	return (FLOAT)mouseState.lZ;
}

//---------------------------------------------------------------------
// Name:DirectInputClass::IsKeyUpRight                                 
// Desc:判断按键是否刚刚弹起         
//---------------------------------------------------------------------
BOOL DirectInputClass::IsKeyUpRight(UINT key)
{
	if(~(keyboardState[key]&0x80)&(lastKeyboardState[key]&0x80))//如果上次按键被按下，现在按键被弹起
		return TRUE;
	return FALSE;
}

BOOL DirectInputClass::IsKeyDownRight(UINT key)
{
	if((keyboardState[key]&0x80)&~(lastKeyboardState[key]&0x80))//如果上次按键被按下，现在按键被弹起
		return TRUE;
	return FALSE;
}

//----------------------------------------------------------------------
// Name:DirectInput::IsMouseButtonUpRight
// Desc:判断鼠标按键是否被刚刚弹起       
//----------------------------------------------------------------------
BOOL DirectInputClass::IsMouseButtonUpRight(UINT key)
{
	if(~(mouseState.rgbButtons[key]&0x80)&(lastMouseState.rgbButtons[key]&0x80))//如果上次鼠标按键按下，现在鼠标按键被弹起
		return TRUE;
	return FALSE;
}
BOOL DirectInputClass::IsMouseButtonDownRight(UINT key)
{
	if((mouseState.rgbButtons[key]&0x80)&~(lastMouseState.rgbButtons[key]&0x80))//如果上次鼠标按键按下，现在鼠标按键被弹起
		return TRUE;
	return FALSE;
}