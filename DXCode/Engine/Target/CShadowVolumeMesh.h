//*******************************************************
// CreateFor 为阴影体做准备，拆分模型
// CreateBy Mailko
// CreateDate 2016-12-11
//*******************************************************
#ifndef _CSHADOW_VOLUME_MESH_
#define _CSHADOW_VOLUME_MESH_

#include "Common.h"
#include "d3d9.h"
#include "d3dx9.h"

struct SHADOW_VOLUEM_D3DVERTEX
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
};

//保证所有的边都有公共边，为了Mesh闭合
struct CEdgeMapping
{
	int m_anOldEdge[2];  // vertex index of the original edge
	int m_aanNewEdge[2][2]; // vertex indexes of the new edge
							// First subscript = index of the new edge
							// Second subscript = index of the vertex for the edge
public:
	CEdgeMapping()
	{
		FillMemory(m_anOldEdge, sizeof(m_anOldEdge), -1);
		FillMemory(m_aanNewEdge, sizeof(m_aanNewEdge), -1);
	}
};

class DLL_API CShadowVolumeMesh
{
private:
	// 模型
	LPDIRECT3DVERTEXBUFFER9       m_pVB;                // 顶点缓冲区
	LPDIRECT3DINDEXBUFFER9        m_pIB;                // 顶点缓冲区指针
	DWORD                         m_dwNumIndices;       // 顶点的索引个数
	DWORD                         m_dwNumVertices;      // 顶点个数
	LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // 顶点声明接口
														//
	LPD3DXMESH                    m_pMesh;    //
	D3DXVECTOR3                   m_vCenter;
	FLOAT                         m_fObjectRadius;

	LPD3DXEFFECT                  m_pEffect;
	
	LPD3DXMESH GenerateShadowMesh(LPD3DXMESH OriMesh, IDirect3DDevice9* pDevice);
	INT FindEdgeInMappingTable(INT Vertex1, INT Vertex2, CEdgeMapping* pMapping, INT  MappingCount);

public:
	CShadowVolumeMesh();
	~CShadowVolumeMesh();
	//
	float GetRadius() { return m_fObjectRadius; }
	void  SetLightParament(D3DXVECTOR4* vPosition);
	//
	BOOL    OnDeviceCreate(LPD3DXMESH OriMesh, IDirect3DDevice9* pDevice);
	void    OnFrameMove(const D3DXVECTOR3 *LightPosition, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime);
	void    OnFrameRender(IDirect3DDevice9* pd3dDevice);

};

#endif