#ifndef _CRENDEROBJECT_H
#define _CRENDEROBJECT_H

#include "ShmFile.h"
#include "d3d9.h"
#include "d3dx9.h"

struct RENDEROBJECT_D3DVERTEX
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	float u, v;
};

class DLL_API CRenderObject
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
	IDirect3DTexture9*            m_pColorTexture;
	// Ч��
	LPD3DXEFFECT                  m_pEffect;            // Ч���ӿ�
														//
	int                           m_iResNum;            // ����Ⱦ�������Դ��
	SRESOURCEINFO*                m_pResourceInfo;      // ������Դ����
	HRESULT LoadResource(TCHAR* FileName);
	TCHAR* GetResourceName(RESTYPE enuResType);
public:
	CRenderObject();
	~CRenderObject();
	LPD3DXMESH GetMesh() { return m_pMeshSysMemory; }
	//
	float GetRadius() { return m_fObjectRadius; }
	void  SetLightParament(D3DXVECTOR4* vPosition);
	//
	void    Init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice, TCHAR* FileName);
	HRESULT OnResetDevice();
	void    OnFrameMove(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime);
	void    OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void    OnLostDevice();
	void    OnDestroyDevice();

	//��������
	void    SetConstantTexture(LPDIRECT3DTEXTURE9 tex);

	//{{for ͶӰ����
	void SetProjectMatrix(const D3DXMATRIX matWorld, const D3DXMATRIX& matView, const D3DXMATRIX &matProject);
	//End}}

	//{{for ��������������
	void SetViewMatrix(const D3DXMATRIX& matView);
	//End}}

	//{{For �������ͼ
	void SetShadowMap(LPDIRECT3DTEXTURE9 shadowmap);
	void SetLightMatrix(const D3DXMATRIXA16 &matWorld, const D3DXMATRIXA16& matView, const D3DXMATRIXA16& matProject);
	void SetProjectDistance(float fNear, float fFar);
	//}}
};

#endif