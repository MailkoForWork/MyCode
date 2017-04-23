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
	// 模型
	LPDIRECT3DVERTEXBUFFER9       m_pVB;                // 顶点缓冲区
	LPDIRECT3DINDEXBUFFER9        m_pIB;                // 顶点缓冲区指针
	DWORD                         m_dwNumIndices;       // 顶点的索引个数
	DWORD                         m_dwNumVertices;      // 顶点个数
	LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // 顶点声明接口
														//
	LPD3DXMESH                    m_pMeshSysMemory;    //
	D3DXVECTOR3                   m_vCenter;
	FLOAT                         m_fObjectRadius;
	// 纹理
	IDirect3DTexture9*            m_pColorTexture;
	// 效果
	LPD3DXEFFECT                  m_pEffect;            // 效果接口
														//
	int                           m_iResNum;            // 本渲染对象的资源数
	SRESOURCEINFO*                m_pResourceInfo;      // 保存资源名字
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

	//传递纹理
	void    SetConstantTexture(LPDIRECT3DTEXTURE9 tex);

	//{{for 投影纹理
	void SetProjectMatrix(const D3DXMATRIX matWorld, const D3DXMATRIX& matView, const D3DXMATRIX &matProject);
	//End}}

	//{{for 绘制立方体纹理
	void SetViewMatrix(const D3DXMATRIX& matView);
	//End}}

	//{{For 深度纹理图
	void SetShadowMap(LPDIRECT3DTEXTURE9 shadowmap);
	void SetLightMatrix(const D3DXMATRIXA16 &matWorld, const D3DXMATRIXA16& matView, const D3DXMATRIXA16& matProject);
	void SetProjectDistance(float fNear, float fFar);
	//}}
};

#endif