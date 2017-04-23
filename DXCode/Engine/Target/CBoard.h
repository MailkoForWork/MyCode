//==========================================
//  CreateBy Mailko
//  CreateFor ����һ��ƽ��������
//  Time     2016-11-27
//==========================================

#ifndef _CBOARD_H_
#define _CBOARD_H_
#include"Common.h"
struct BOARD_D3DVERTEX
{
	float x, y, z, w;
	DWORD color;
	float u, v;
};

class DLL_API CBoard
{
private:
	LPDIRECT3DTEXTURE9 m_Tex;
	LPDIRECT3DVERTEXBUFFER9 m_Vertexs;
	LPDIRECT3DINDEXBUFFER9 m_Indexs;
	DWORD m_NumVertex;
	DWORD m_NumIndex;
	LPDIRECT3DVERTEXDECLARATION9 m_Desc;
	LPD3DXEFFECT       m_Effect;

	HRESULT CreateSquare(IDirect3DDevice9 *pDevice);

public:
	CBoard();
	~CBoard();
	//
	void    Init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice);
	void    OnFrameMove(float g_fTime, D3DXVECTOR4 Position);
	void    OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void    OnDestroyDevice();

	//����������λ�÷�����ɫ���ڼ���
	void SetTranslateMatrix(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject);

	//�ı���ʾ�������������ļ����µ���һ��
	void ChangeTexture(LPDIRECT3DTEXTURE9 pTexture);

	//���ô���������С
	void SetTextureSize(INT Width, INT Height);
	VOID SetMosaicSize(INT Width, INT Height);
};

#endif