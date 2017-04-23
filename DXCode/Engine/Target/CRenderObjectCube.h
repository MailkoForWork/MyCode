//************************************************************************
//   CreateBy Mailko
//   Date 2016-12-07
//	 For ������������Ⱦ�� ��Ҫ���ڶ�������������Ĳ����������ɫ��ʹ��
//   Tip ��CRenderObject�����ƣ�����ֻ�������������,����"@dif"�鿴
//*************************************************************************

#ifndef _CRENDER_OBJECT_CUBE_
#define _CRENDER_OBJECT_CUBE_

#include "ShmFile.h"
#include "d3d9.h"
#include "d3dx9.h"

struct RENDEROBJECTCUBE_D3DVERTEX
{
	D3DXVECTOR3 Position;
	float u, v;
};

class DLL_API CRenderObjectCube
{
private:
	// ģ��
	LPDIRECT3DVERTEXBUFFER9       m_pVB;                // ���㻺����
	LPDIRECT3DINDEXBUFFER9        m_pIB;                // ���㻺����ָ��
	DWORD                         m_dwNumIndices;       // �������������
	DWORD                         m_dwNumVertices;      // �������
	LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // ���������ӿ�
														//
	LPD3DXMESH                    m_pMeshSysMemory;    //
	D3DXVECTOR3                   m_vCenter;
	FLOAT                         m_fObjectRadius;
	// ����
	IDirect3DCubeTexture9*        m_pCubeTexture;//@dif
	// Ч��
	LPD3DXEFFECT                  m_pEffect;            // Ч���ӿ�
														//
	int                           m_iResNum;            // ����Ⱦ�������Դ��
	SRESOURCEINFO*                m_pResourceInfo;      // ������Դ����
	HRESULT LoadResource(TCHAR* FileName);
	TCHAR* GetResourceName(RESTYPE enuResType);
public:
	CRenderObjectCube();
	~CRenderObjectCube();
	//
	float GetRadius() { return m_fObjectRadius; }

	//
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice, TCHAR* FileName);
	HRESULT OnResetDevice();
	void    OnFrameMove(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime);
	void    OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void    OnLostDevice();
	void    OnDestroyDevice();

	//{{for ������������ƣ� ÿһ����Ҫ���������ӽǾ���
	void SetViewMatrix(const D3DXMATRIX& matView);
	HRESULT SetTexture(LPDIRECT3DCUBETEXTURE9 pTex);
	//}}
};

#endif