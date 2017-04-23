//**********************************************//
// Name:Direct3DInputClass.h                    //
// Desc:��ȡ�豸����                            //
// Author:mailko                                //
// Date:2015.04.26                              //
//**********************************************//
#ifndef _DIRECTINPUTCLASS_H_
#define _DIRECTINPUTCLASS_H_
#include<dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#include"Common.h"
//������������
class DLL_API DirectInputClass{
//��Ա����������
private:
	IDirectInput8 *pDI8;
	IDirectInputDevice8 *pMouseDI;
	IDirectInputDevice8 *pKeyboardDI;
	char keyboardState[256];
	char lastKeyboardState[256];//�ϴεļ���״̬
	DIMOUSESTATE mouseState;
	DIMOUSESTATE lastMouseState;//�ϴε����״̬
//��Ա����������
public:
	DirectInputClass();//���캯����ֻ�����Ա�����ĳ�ʼ��
	~DirectInputClass();//���������������Ա�������ͷ�
	HRESULT DirectInputInit(HWND hwnd,HINSTANCE hInstance);//��ȡ�����豸�ӿ�
	HRESULT GetInput();//��ȡ��ǰ���룬��������״̬
	BOOL IsKeyDown(UINT key);//�ж�ĳ���Ƿ���
	BOOL IsMouseButtonDown(UINT button);//�ж����ĳ���Ƿ���
	BOOL IsKeyDownRight(UINT key);
	BOOL IsKeyUpRight(UINT key);//�ж�ĳ���Ƿ�ձ�����
	BOOL IsMouseButtonUpRight(UINT button);//�ж����ĳ���Ƿ�ձ��ͷ�
	BOOL IsMouseButtonDownRight(UINT button);
	//��������������ƶ�����
	float MouseDX();
	float MouseDY();
	float MouseDZ();
};



#endif