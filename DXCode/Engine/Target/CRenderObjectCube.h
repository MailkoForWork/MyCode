//************************************************************************
//   CreateBy Mailko
//   Date 2016-12-07
//	 For 立方体纹理渲染， 主要在于对于立方体纹理的操作和相关着色器使用
//   Tip 与CRenderObject类相似，区别只在于纹理差别操作,可搜"@dif"查看
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
	IDirect3DCubeTexture9*        m_pCubeTexture;//@dif
	// 效果
	LPD3DXEFFECT                  m_pEffect;            // 效果接口
														//
	int                           m_iResNum;            // 本渲染对象的资源数
	SRESOURCEINFO*                m_pResourceInfo;      // 保存资源名字
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

	//{{for 立方体纹理绘制， 每一面需要单独设置视角矩阵
	void SetViewMatrix(const D3DXMATRIX& matView);
	HRESULT SetTexture(LPDIRECT3DCUBETEXTURE9 pTex);
	//}}
};

#endif