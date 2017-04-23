#define DLL_IMPLEMENT
#include "CShadowVolumeMesh.h"

CShadowVolumeMesh::CShadowVolumeMesh()
{
	m_pVB = NULL;
	m_pIB = NULL;
	m_dwNumIndices = 0;
	m_dwNumVertices = 0;
	m_pDecl = NULL;
	m_pMesh = NULL;
	m_pEffect = NULL;
}

CShadowVolumeMesh::~CShadowVolumeMesh()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pEffect);
}

BOOL CShadowVolumeMesh::OnDeviceCreate(LPD3DXMESH OriMesh, IDirect3DDevice9* pDevice)
{
	HRESULT hr = S_OK;
	m_pMesh = GenerateShadowMesh(OriMesh, pDevice);//生成对应的ShadowVolume;
	m_dwNumIndices = m_pMesh->GetNumFaces() * 3;
	m_dwNumVertices = m_pMesh->GetNumVertices();
	HR(m_pMesh->GetIndexBuffer(&m_pIB));
	HR(m_pMesh->GetVertexBuffer(&m_pVB));

	//顶点描述
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL , 0 },
		D3DDECL_END()
	};
	// --------- 因为装入的 Mesh 的顶点结构不是我们需要的，因此按我们需要的格式复制一个 ----------
	if (FAILED(hr = pDevice->CreateVertexDeclaration(decl, &m_pDecl))) return hr;

	//加载效果文件
	LPD3DXBUFFER pCode = NULL;
	if (FAILED(hr = D3DXCreateEffectFromFile(pDevice, TEXT("FileFX.fx"), NULL, NULL, 0, NULL, &m_pEffect, &pCode)))
	{
		LPVOID pBuffErr = pCode->GetBufferPointer();
		char* ErrInfo = (char*)pBuffErr;
		MessageBox(NULL, ErrInfo, "", 0);
		pCode->Release();
		return hr;
	}
	return TRUE;
}

void CShadowVolumeMesh::OnFrameMove(const D3DXVECTOR3* LightPosition, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime)
{
	HRESULT hr = S_OK;
	D3DXVECTOR4 v4LightPosition = D3DXVECTOR4(LightPosition->x, LightPosition->y, LightPosition->z, 1);

	hr = m_pEffect->SetVector("g_LightPosition", &v4LightPosition);
	hr = m_pEffect->SetMatrix("g_matWorld", pWorld);
	hr = m_pEffect->SetMatrix("g_matView", pView);
	hr = m_pEffect->SetMatrix("g_matProject", pProject);
}

void CShadowVolumeMesh::OnFrameRender(IDirect3DDevice9* pd3dDevice)
{
	HRESULT hr = S_OK;

	hr = pd3dDevice->SetVertexDeclaration(m_pDecl);
	hr = pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(SHADOW_VOLUEM_D3DVERTEX));
	hr = pd3dDevice->SetIndices(m_pIB);

	UINT iPass, cPasses;
	hr = m_pEffect->Begin(&cPasses, 0);
	for (iPass = 0; iPass < cPasses; iPass++)
	{
		hr = m_pEffect->BeginPass(iPass);
		hr = pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, m_dwNumIndices);
		hr = m_pEffect->EndPass();
	}
	hr = m_pEffect->End();
}

LPD3DXMESH CShadowVolumeMesh::GenerateShadowMesh(LPD3DXMESH OriMesh, IDirect3DDevice9* pDevice)
{
	if (!OriMesh || !pDevice)
	{
		OutputDebugString("Error  CShadowVolumeMesh::OnDeviceCreate Param is Null");
		return NULL;
	}

	LPD3DXMESH pOriMesh = OriMesh;
	LPD3DXMESH pOutMesh = NULL;
	HRESULT hr = S_OK;

	//将Mesh先拆成相应的格式
	D3DVERTEXELEMENT9 ShadowVertexDesc[3] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		D3DDECL_END()
	};
	if (FAILED(pOriMesh->CloneMesh(D3DXMESH_32BIT, ShadowVertexDesc, pDevice, &pOutMesh)))
	{
		return NULL;
	};

	//====================={{将每一个面独立
	pOriMesh = pOutMesh;
	pOutMesh = NULL;

	DWORD* pOriAdj = new DWORD[3 * pOriMesh->GetNumFaces()];
	DWORD* pOriPointRep = new DWORD[pOriMesh->GetNumFaces() * 3];
	if (!pOriAdj || !pOriPointRep)
	{
		SAFE_DELETE(pOriAdj);
		SAFE_DELETE(pOriPointRep);
		OutputDebugString("Error 分配内存失败");
		return NULL;
	}

	//获取邻接信息
	if (FAILED(pOriMesh->GenerateAdjacency(0.01, pOriAdj)))
	{
		return NULL;
	}
	if (FAILED(pOriMesh->ConvertAdjacencyToPointReps(pOriAdj, pOriPointRep)))
	{
		return NULL;
	}

	//创建一个足够容量的Mesh
	if (FAILED(D3DXCreateMesh(pOriMesh->GetNumFaces() + pOriMesh->GetNumFaces() * 3 * 2, /*面数*/
		pOriMesh->GetNumFaces() * 3,
		D3DXMESH_32BIT, ShadowVertexDesc, pDevice, &pOutMesh)))
	{
		return NULL;
	}
	INT EdgeMapCount = pOriMesh->GetNumFaces() * 3;
	//CEdgeMapping *EdgeMaps = new CEdgeMapping[EdgeMapCount];
	CEdgeMapping EdgeMaps[36];
	INT iValidEdgeMap = 0;
	if (!EdgeMaps)
	{
		return NULL;
	}

	//获取顶点相关指针
	SHADOW_VOLUEM_D3DVERTEX* pOriVertex;
	DWORD* pOriIndex;
	SHADOW_VOLUEM_D3DVERTEX* pOutVertex;
	DWORD* pOutIndex;

	hr = pOriMesh->LockVertexBuffer(0, (LPVOID*)&pOriVertex);
	hr = pOriMesh->LockIndexBuffer(0, (LPVOID*)&pOriIndex);
	hr = pOutMesh->LockVertexBuffer(0, (LPVOID*)&pOutVertex);
	hr = pOutMesh->LockIndexBuffer(0, (LPVOID*)&pOutIndex);

	memset(pOutIndex, 0, sizeof(DWORD) * pOriMesh->GetNumFaces() + pOriMesh->GetNumFaces() * 3 * 2);
	//遍历所有面，并拆分面,基本上每一个原始的面上的三个顶点和索引都是独立的。
	SHADOW_VOLUEM_D3DVERTEX* pCurVertex = pOutVertex;
	DWORD iOutIndex = 0;//写入的当前索引
	DWORD iOutVertex = 0; //当前写入的顶点

	for (int OriFaceIndex = 0; OriFaceIndex < pOriMesh->GetNumFaces(); OriFaceIndex++)
	{
		//处理原始面，因为要独立，所以不可能有公共点。
		DWORD iOriIndex = OriFaceIndex * 3; //面的第一个顶点索引
		SHADOW_VOLUEM_D3DVERTEX Vertex0 = pOriVertex[pOriIndex[iOriIndex]];
		SHADOW_VOLUEM_D3DVERTEX Vertex1 = pOriVertex[pOriIndex[iOriIndex + 1]];
		SHADOW_VOLUEM_D3DVERTEX Vertex2 = pOriVertex[pOriIndex[iOriIndex + 2]];
		D3DXVECTOR3 v1, v2;
		D3DXVECTOR3 vNormal;
		v1 = Vertex1.Position - Vertex0.Position;
		v2 = Vertex2.Position - Vertex0.Position;
		D3DXVec3Cross(&vNormal, &v1, &v2);
		D3DXVec3Normalize(&vNormal, &vNormal);
		Vertex0.Normal = vNormal;
		Vertex1.Normal = vNormal;
		Vertex2.Normal = vNormal;
		pOutVertex[iOutVertex] = Vertex0;
		pOutVertex[iOutVertex + 1] = Vertex1;
		pOutVertex[iOutVertex + 2] = Vertex2;
		pOutIndex[iOutIndex++] = iOutVertex;
		pOutIndex[iOutIndex++] = iOutVertex + 1;
		pOutIndex[iOutIndex++] = iOutVertex + 2;
		DWORD iEdgeVertexBegin = iOutVertex;
		iOutVertex += 3;


		int nEdgeIndex = 0;
		int nVertIndex[3] = {//pOriPointRep以顶点序号为序，对应每一个顶点代表的位置代表，以点为序，
			pOriPointRep[pOriIndex[iOriIndex]],
			pOriPointRep[pOriIndex[iOriIndex + 1]],
			pOriPointRep[pOriIndex[iOriIndex + 2]]
		};
		DWORD temp[36];
		memcpy(temp, pOriPointRep, sizeof(DWORD) * 36);
		//第一条边测试公共边
		nEdgeIndex = FindEdgeInMappingTable(nVertIndex[0], nVertIndex[1], EdgeMaps, EdgeMapCount);
		if (EdgeMaps[nEdgeIndex].m_anOldEdge[0] == -1 && EdgeMaps[nEdgeIndex].m_anOldEdge[1] == -1)//该边未被记录在案
		{
			EdgeMaps[nEdgeIndex].m_anOldEdge[0] = nVertIndex[0];
			EdgeMaps[nEdgeIndex].m_anOldEdge[1] = nVertIndex[1];
			EdgeMaps[nEdgeIndex].m_aanNewEdge[0][0] = iEdgeVertexBegin;
			EdgeMaps[nEdgeIndex].m_aanNewEdge[0][1] = iEdgeVertexBegin + 1;
			iValidEdgeMap++;
		}
		else //已经有相同边记录在案,在公共边之间撑起两个三角面
		{
			EdgeMaps[nEdgeIndex].m_aanNewEdge[1][0] = iEdgeVertexBegin;
			EdgeMaps[nEdgeIndex].m_aanNewEdge[1][1] = iEdgeVertexBegin + 1;
			
			//法线不同，判断两个面为断面
			D3DXVECTOR3 normal = pOutVertex[EdgeMaps[nEdgeIndex].m_aanNewEdge[1][0]].Normal;
			D3DXVECTOR3 normal2 = pOutVertex[EdgeMaps[nEdgeIndex].m_aanNewEdge[0][0]].Normal;
			if (!(abs(D3DXVec3Dot(&normal, &normal2)) < 0.01))
			{
				//第一个三角形
				pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[0][0];
				pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[1][1];
				pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[0][1];

				//第二个三角形
				pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[0][1];
				pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[1][1];
				pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[1][0];
			}

			//将该公共边删除 这里默认一个边只有两个三角形共边。
			EdgeMaps[nEdgeIndex] = EdgeMaps[iValidEdgeMap - 1];
			FillMemory(&EdgeMaps[iValidEdgeMap - 1], sizeof(CEdgeMapping), 0xFF);
			--iValidEdgeMap;
		}

		//第二条边测试公共边
		nEdgeIndex = FindEdgeInMappingTable(nVertIndex[1], nVertIndex[2], EdgeMaps, EdgeMapCount);
		if (EdgeMaps[nEdgeIndex].m_anOldEdge[0] == -1 && EdgeMaps[nEdgeIndex].m_anOldEdge[1] == -1)//该边未被记录在案
		{
			EdgeMaps[nEdgeIndex].m_anOldEdge[0] = nVertIndex[1];
			EdgeMaps[nEdgeIndex].m_anOldEdge[1] = nVertIndex[2];
			EdgeMaps[nEdgeIndex].m_aanNewEdge[0][0] = iEdgeVertexBegin + 1;
			EdgeMaps[nEdgeIndex].m_aanNewEdge[0][1] = iEdgeVertexBegin + 2;
			iValidEdgeMap++;
		}
		else //已经有相同边记录在案,在公共边之间撑起两个三角面
		{
			EdgeMaps[nEdgeIndex].m_aanNewEdge[1][0] = iEdgeVertexBegin + 1;
			EdgeMaps[nEdgeIndex].m_aanNewEdge[1][1] = iEdgeVertexBegin + 2;

			//第一个三角形
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[0][0];
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[1][1];
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[0][1];

			//第二个三角形
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[0][1];
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[1][1];
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[1][0];

			//将该公共边删除 这里默认一个边只有两个三角形共边。
			EdgeMaps[nEdgeIndex] = EdgeMaps[iValidEdgeMap - 1];
			FillMemory(&EdgeMaps[iValidEdgeMap - 1], sizeof(CEdgeMapping), 0xFF);
			--iValidEdgeMap;
		}

		//第三条边测试公共边
		nEdgeIndex = FindEdgeInMappingTable(nVertIndex[2], nVertIndex[0], EdgeMaps, EdgeMapCount);
		if (EdgeMaps[nEdgeIndex].m_anOldEdge[0] == -1 && EdgeMaps[nEdgeIndex].m_anOldEdge[1] == -1)//该边未被记录在案
		{
			EdgeMaps[nEdgeIndex].m_anOldEdge[0] = nVertIndex[2];
			EdgeMaps[nEdgeIndex].m_anOldEdge[1] = nVertIndex[0];
			EdgeMaps[nEdgeIndex].m_aanNewEdge[0][0] = iEdgeVertexBegin + 2;
			EdgeMaps[nEdgeIndex].m_aanNewEdge[0][1] = iEdgeVertexBegin;
			iValidEdgeMap++;
		}
		else //已经有相同边记录在案,在公共边之间撑起两个三角面
		{
			EdgeMaps[nEdgeIndex].m_aanNewEdge[1][0] = iEdgeVertexBegin + 2;
			EdgeMaps[nEdgeIndex].m_aanNewEdge[1][1] = iEdgeVertexBegin;

			//第一个三角形
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[0][0];
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[1][1];
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[0][1];

			//第二个三角形
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[0][1];
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[1][1];
			pOutIndex[iOutIndex++] = EdgeMaps[nEdgeIndex].m_aanNewEdge[1][0];

			//将该公共边删除 这里默认一个边只有两个三角形共边。
			EdgeMaps[nEdgeIndex] = EdgeMaps[iValidEdgeMap - 1];
			FillMemory(&EdgeMaps[iValidEdgeMap - 1], sizeof(CEdgeMapping), 0xFF);
			--iValidEdgeMap;
		}
	}
	//=======================将每一个面独立}}

	//====================={{闭合Mesh:（经过此操作不代表Mesh真正的闭合，只是在每一个边缘都具有可扩展面，
	//理论 在EdgeMaps中剩余的边都没有公共边（这是由于原来的Mesh就不是完全封闭的),拥有公共点的边会将生成一个面，
	//然后将此面当成原始面，并在该面两边建立可拉伸面
	//
	for (int mapIndex = 0; mapIndex < iValidEdgeMap; mapIndex++)
	{
		CEdgeMapping& FirstMap = EdgeMaps[mapIndex];
		if (FirstMap.m_anOldEdge[0] == -1 || FirstMap.m_anOldEdge[1] == -1)
		{
			OutputDebugStringA("Error: Map中不应该存在空边，Map数组生成有误");
			continue;
		}
		if (FirstMap.m_aanNewEdge[1][0] != -1 || FirstMap.m_aanNewEdge[1][1] != -1)//已处理过的
		{
			continue;
		}
		for (int mapIndex2 = mapIndex + 1; mapIndex2 < iValidEdgeMap; mapIndex2++)//遍历接下来的点进行公共点查询
		{
			CEdgeMapping& SecondMap = EdgeMaps[mapIndex2];
			if (FirstMap.m_aanNewEdge[1][0] != -1 || FirstMap.m_aanNewEdge[1][1] != -1)//已处理过的
			{
				continue;
			}
			INT nSamePointCount = 0;
			INT nDirection = 0;//记录将哪个边作为首边
			if (FirstMap.m_anOldEdge[0] == SecondMap.m_anOldEdge[0]
				|| FirstMap.m_anOldEdge[1] == SecondMap.m_anOldEdge[1])
			{
				OutputDebugStringA("Warning:理论上Mesh不应该出现这种情况，这两边的方向导致面的方向无法确定");
			}
			if ( FirstMap.m_anOldEdge[1] == SecondMap.m_anOldEdge[0])
			{
				nDirection = 0;//First的边作为原来的首边
				nSamePointCount++;
			}
			if (FirstMap.m_anOldEdge[0] == SecondMap.m_anOldEdge[1])
			{
				nDirection = 1;
				nSamePointCount++;
			}
			if (nSamePointCount == 0)
			{
				continue;
			}
			else if (nSamePointCount == 1)
			{
				//拥有一个公共点
				//将两个边做成一个面,新的面的点顺序与原来的相反
				INT iTriVertexIndex0 = nDirection == 0 ? SecondMap.m_aanNewEdge[0][1]
					: FirstMap.m_aanNewEdge[0][1];
				INT iTriVertexIndex1 = nDirection == 0 ? SecondMap.m_aanNewEdge[0][0]
					: FirstMap.m_aanNewEdge[0][0];
				INT iTriVertexIndex2 = nDirection == 0 ? FirstMap.m_aanNewEdge[0][0]
					: SecondMap.m_aanNewEdge[0][0];
				SHADOW_VOLUEM_D3DVERTEX vertex0 = pOutVertex[iTriVertexIndex0];
				SHADOW_VOLUEM_D3DVERTEX vertex1 = pOutVertex[iTriVertexIndex1];
				SHADOW_VOLUEM_D3DVERTEX vertex2 = pOutVertex[iTriVertexIndex2];
				D3DXVECTOR3 vec1 = vertex1.Position - vertex0.Position;
				D3DXVECTOR3 vec2 = vertex2.Position - vertex0.Position;
				D3DXVECTOR3 normal;
				D3DXVec3Cross(&normal, &vec1, &vec2);
				D3DXVec3Normalize(&normal, &normal);
				vertex0.Normal = normal;
				vertex1.Normal = normal;
				vertex2.Normal = normal;
				pOutVertex[iOutVertex++] = vertex0;
				pOutVertex[iOutVertex++] = vertex1;
				pOutVertex[iOutVertex++] = vertex2;
				INT iTriVertex = iOutVertex - 3;
				pOutIndex[iOutIndex++] = iTriVertex + 0;
				pOutIndex[iOutIndex++] = iTriVertex + 1;
				pOutIndex[iOutIndex++] = iTriVertex + 2;



				//创建拉伸矩形,FistMap边产生的边
				{
					FirstMap.m_aanNewEdge[1][0] = iTriVertex;
					FirstMap.m_aanNewEdge[1][1] = iTriVertex + 1;
					const INT QuadVertex0 = FirstMap.m_aanNewEdge[1][1];
					const INT QuadVertex1 = FirstMap.m_aanNewEdge[1][0];
					const INT QuadVertex2 = FirstMap.m_aanNewEdge[0][1];
					const INT QuadVertex3 = FirstMap.m_aanNewEdge[0][0];

					pOutIndex[iOutIndex++] = QuadVertex0;
					pOutIndex[iOutIndex++] = QuadVertex1;
					pOutIndex[iOutIndex++] = QuadVertex2;
					pOutIndex[iOutIndex++] = QuadVertex2;
					pOutIndex[iOutIndex++] = QuadVertex3;
					pOutIndex[iOutIndex++] = QuadVertex0;
				}
				//创建第二个拉伸矩形
				{
					SecondMap.m_aanNewEdge[1][0] = iTriVertex + 1;
					SecondMap.m_aanNewEdge[1][1] = iTriVertex + 2;
					const INT QuadVertex0 = SecondMap.m_aanNewEdge[1][1];
					const INT QuadVertex1 = SecondMap.m_aanNewEdge[1][0];
					const INT QuadVertex2 = SecondMap.m_aanNewEdge[0][1];
					const INT QuadVertex3 = SecondMap.m_aanNewEdge[0][0];
					pOutIndex[iOutIndex++] = QuadVertex0;
					pOutIndex[iOutIndex++] = QuadVertex1;
					pOutIndex[iOutIndex++] = QuadVertex2;
					pOutIndex[iOutIndex++] = QuadVertex2;
					pOutIndex[iOutIndex++] = QuadVertex3;
					pOutIndex[iOutIndex++] = QuadVertex0;
				}
			}
			else if(nSamePointCount >= 2)
			{
				OutputDebugStringA("Error:理论上不应该进到这 nSamePointCount >= 2");
			}
		}
	}
	//=====================闭合Mesh}}

	//清理工作：
	pOriMesh->UnlockVertexBuffer();
	pOriVertex = NULL;
	pOriMesh->UnlockIndexBuffer();
	pOriIndex = NULL;


	//====================={{简化空间
	pOriMesh = pOutMesh;
	pOriVertex = pOutVertex;
	pOriIndex = pOutIndex;
	INT nTotalOriIndex = iOutIndex;
	INT nTotalOriVertex = iOutVertex;
	BOOL bNeed32Bit = nTotalOriIndex > 65535;
	iOutIndex = 0;
	iOutVertex = 0;
	pOutVertex = NULL;
	pOutIndex = NULL;
	pOutMesh = NULL;
	if (FAILED(D3DXCreateMesh(nTotalOriIndex / 3, 
		nTotalOriVertex, bNeed32Bit ? D3DXMESH_32BIT : 0,
		ShadowVertexDesc, pDevice, &pOutMesh)))
	{
		return NULL;
	}

	LPVOID pVoidOutIndex = NULL;
	pOutMesh->LockIndexBuffer(nTotalOriIndex > 65535 ? D3DXMESH_32BIT : 0, (LPVOID*)&pVoidOutIndex);
	pOutMesh->LockVertexBuffer(0, (LPVOID*)&pOutVertex);

	//复制空间
	memcpy(pOutVertex, pOriVertex, sizeof(SHADOW_VOLUEM_D3DVERTEX) * nTotalOriVertex);
	if (bNeed32Bit)
	{
		memcpy(pVoidOutIndex, pOriIndex, sizeof(DWORD) * nTotalOriIndex);
	}
	else
	{
		WORD* pWordIndex = (WORD*)pVoidOutIndex;
		for (int i = 0; i < nTotalOriIndex; i++)
		{
			pWordIndex[i] = (WORD)pOriIndex[i];
		}
	}
	pOriMesh->UnlockIndexBuffer();
	pOriMesh->UnlockVertexBuffer();
	pOriIndex = NULL;
	pOriVertex = NULL;
	SAFE_RELEASE(pOriMesh);
	pOutMesh->UnlockIndexBuffer();
	pOutMesh->UnlockVertexBuffer();
	pOutVertex = NULL;
	pOriIndex = NULL;
	//====================简化空间}}
	return pOutMesh;
}

INT CShadowVolumeMesh::FindEdgeInMappingTable(INT Vertex0, INT Vertex1, CEdgeMapping* pMapping, INT  MappingCount)
{
	for (INT i = 0; i < MappingCount; i++)
	{
		const CEdgeMapping& EdgeMap = pMapping[i];
		if ((EdgeMap.m_anOldEdge[0] == -1 && EdgeMap.m_anOldEdge[1] == -1) //该线段还未被记录
			|| (EdgeMap.m_anOldEdge[0] == Vertex0 && EdgeMap.m_anOldEdge[1] == Vertex1)
				|| (EdgeMap.m_anOldEdge[1] == Vertex0 && EdgeMap.m_anOldEdge[0] == Vertex1) //该段被记录在案
			)
		{
			return i;
		}
	}
	OutputDebugString("MailkoError CShadowVolumeMesh::FindEdgeInMappingTable");
	return -1;
}