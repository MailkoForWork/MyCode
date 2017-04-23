//***************************************************************//
//     Name:DirectInputClass                                     //
//     Desc:�豸����ӿ��࣬������̺����ĵ�ǰ״̬             //
//***************************************************************//
#define DLL_IMPLEMENT
#include"DirectInputClass.h"
//-------------------------------------------------
// Name:DirectInputClass�������캯��
// Desc: �����Ļ�����ʼ��
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
//Desc:��������
//--------------------------------------------------
DirectInputClass::~DirectInputClass()
{
	SAFE_RELEASE(pMouseDI);
	SAFE_RELEASE(pKeyboardDI);
	SAFE_RELEASE(pDI8);
}

//--------------------------------------------------
//  Name:DirectInputClass::DirectInputInit
//  Desc:��ȡ����豸�ͼ����豸
//--------------------------------------------------

HRESULT DirectInputClass::DirectInputInit(HWND hwnd,HINSTANCE hInstance)
{
	HRESULT hr;
	HR(DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,
		(void**)&pDI8,NULL));
	//��ȡ�����豸��
	HR(pDI8->CreateDevice(GUID_SysKeyboard,&pKeyboardDI,NULL));
	HR(pKeyboardDI->SetDataFormat(&c_dfDIKeyboard));
	HR(pKeyboardDI->SetCooperativeLevel(hwnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE));
	//HR(pKeyboardDI->Acquire());
	//HR(pKeyboardDI->Poll());
	//��ȡ����豸��
	HR(pDI8->CreateDevice(GUID_SysMouse,&pMouseDI,NULL));
	HR(pMouseDI->SetCooperativeLevel(hwnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE));
	HR(pMouseDI->SetDataFormat(&c_dfDIMouse));
	//HR(pMouseDI->Acquire());
	//HR(pMouseDI->Poll());
	return S_OK;
}


//------------------------------------------------
// Name:DirectInputClass::GetInput()
// Desc:��ȡ�����豸��ǰ״̬
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
//  Name��DirectInputClass����IsKeyDown()
//  Desc: �жϸð����Ƿ��ڰ���״̬
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
// Name:DirectInputClass����MouseDX,MouseDY,MouseDX
// Desc:��ȡ����λ�ñ仯
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
// Desc:�жϰ����Ƿ�ոյ���         
//---------------------------------------------------------------------
BOOL DirectInputClass::IsKeyUpRight(UINT key)
{
	if(~(keyboardState[key]&0x80)&(lastKeyboardState[key]&0x80))//����ϴΰ��������£����ڰ���������
		return TRUE;
	return FALSE;
}

BOOL DirectInputClass::IsKeyDownRight(UINT key)
{
	if((keyboardState[key]&0x80)&~(lastKeyboardState[key]&0x80))//����ϴΰ��������£����ڰ���������
		return TRUE;
	return FALSE;
}

//----------------------------------------------------------------------
// Name:DirectInput::IsMouseButtonUpRight
// Desc:�ж���갴���Ƿ񱻸ոյ���       
//----------------------------------------------------------------------
BOOL DirectInputClass::IsMouseButtonUpRight(UINT key)
{
	if(~(mouseState.rgbButtons[key]&0x80)&(lastMouseState.rgbButtons[key]&0x80))//����ϴ���갴�����£�������갴��������
		return TRUE;
	return FALSE;
}
BOOL DirectInputClass::IsMouseButtonDownRight(UINT key)
{
	if((mouseState.rgbButtons[key]&0x80)&~(lastMouseState.rgbButtons[key]&0x80))//����ϴ���갴�����£�������갴��������
		return TRUE;
	return FALSE;
}