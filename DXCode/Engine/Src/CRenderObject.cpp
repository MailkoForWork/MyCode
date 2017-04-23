#define DLL_IMPLEMENT
#include "CRenderObject.h"
#include "Common.h"
#include "d3dx9.h"
#include "d3d9.h"

CRenderObject::CRenderObject()
{
	m_pVB = NULL;
	m_pIB = NULL;
	m_dwNumIndices = 0;
	m_dwNumVertices = 0;
	m_pDecl = NULL;
	m_pMeshSysMemory = NULL;
	m_pEffect = NULL;
	m_pColorTexture = NULL;
}

CRenderObject::~CRenderObject()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pMeshSysMemory);
	SAFE_RELEASE(m_pColorTexture);
}

void CRenderObject::Init()
{

}

HRESULT CRenderObject::OnCreateDevice(IDirect3DDevice9* pd3dDevice, TCHAR* FileName)
{
	HRESULT hr = S_OK;
	if (FAILED(hr = LoadResource(FileName)))
	{
		return hr;
	}
	TCHAR* szFile = GetResourceName(ENU_MODELX);
	
	LPD3DXMESH pMeshSysMemClone = NULL;
	if (FAILED(hr = D3DXLoadMeshFromX(szFile, D3DXMESH_SYSTEMMEM,
		pd3dDevice, NULL, NULL, NULL, NULL, &pMeshSysMemClone)))
	{
		return hr;
	}

	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0  },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0   },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	//Mesh的顶点结构不一致，重新计算；
	if (FAILED(hr = pd3dDevice->CreateVertexDeclaration(decl, &m_pDecl))) return hr;
	if (FAILED(hr = pMeshSysMemClone->CloneMesh(D3DXMESH_MANAGED, decl, pd3dDevice, &m_pMeshSysMemory))) return hr;
	if (FAILED(hr = m_pMeshSysMemory->GetIndexBuffer(&m_pIB))) return hr;
	if (FAILED(hr = m_pMeshSysMemory->GetVertexBuffer(&m_pVB))) return hr;
	m_dwNumIndices = m_pMeshSysMemory->GetNumFaces();
	m_dwNumVertices = m_pMeshSysMemory->GetNumVertices();
	SAFE_RELEASE(pMeshSysMemClone);

	D3DXVECTOR3* pData;
	if(FAILED(hr = m_pMeshSysMemory->LockVertexBuffer(0, (LPVOID*)&pData))) return hr;
	if (FAILED(hr = D3DXComputeBoundingSphere(pData, m_pMeshSysMemory->GetNumVertices(), D3DXGetFVFVertexSize(m_pMeshSysMemory->GetFVF()), &m_vCenter, &m_fObjectRadius))) return hr;
	if (FAILED(hr = m_pMeshSysMemory->UnlockVertexBuffer())) return FALSE;

	//加载效果文件
	szFile = GetResourceName(ENU_FXFILE);
	LPD3DXBUFFER pCode = NULL;
	if (FAILED(hr = D3DXCreateEffectFromFile(pd3dDevice, szFile, NULL, NULL, 0, NULL, &m_pEffect, &pCode)))
	{
		LPVOID pBuffErr = pCode->GetBufferPointer();
		char* ErrInfo = (char*)pBuffErr;
		MessageBox(NULL, ErrInfo, "", 0);
		pCode->Release();
		return hr;
	}

	szFile = GetResourceName(ENU_TEXCOLORMAP);
	if (FAILED(hr = D3DXCreateTextureFromFileEx(pd3dDevice, szFile, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0,
		NULL, NULL, &m_pColorTexture))) return hr;

	OnResetDevice();
	return hr;
}

HRESULT CRenderObject::OnResetDevice()
{
	HRESULT hr = S_OK;
	if (FAILED(hr = m_pEffect->SetTechnique("DefaultTech"))) return hr;
	if( FAILED( hr = m_pEffect->SetTexture( "g_ColorTexture", m_pColorTexture ))) return hr;
	return hr;
}

void CRenderObject::OnFrameMove(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime)
{
	HRESULT hr = S_OK;
	hr = m_pEffect->SetMatrix("g_matWorld", pWorld);
	hr = m_pEffect->SetMatrix("g_matView", pView);
	hr = m_pEffect->SetMatrix("g_matProject", pProject);
}

void CRenderObject::OnFrameRender(IDirect3DDevice9* pd3dDevice)
{
	HRESULT hr = S_OK;

	hr = pd3dDevice->SetVertexDeclaration(m_pDecl);
	hr = pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(RENDEROBJECT_D3DVERTEX));
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

void CRenderObject::OnLostDevice()
{
	if (m_pEffect)   m_pEffect->OnLostDevice();
}

void CRenderObject::OnDestroyDevice()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pMeshSysMemory);
	SAFE_RELEASE(m_pColorTexture);
}

//----------------------------------------------------------------------------
// LoadResource
// 功能：将本渲染对象的文件（*.shm）装入，分析供 GetResourceName 使用
//----------------------------------------------------------------------------
HRESULT CRenderObject::LoadResource(TCHAR* FileName)
{
	// 打开文件
	BOOL bReturn;
	HANDLE hAndle;
	DWORD  uByte;
	// 文件存在否
	//---------------------- 调试用 --------------------------
	//TCHAR strPath[MAX_PATH];
	//GetCurrentDirectory( MAX_PATH , strPath );
	if (INVALID_HANDLE_VALUE == (hAndle = CreateFile(FileName,
		GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL)))    return E_FAIL;
	// ----------------- 检查文件头 --------------------
	SSHADERMODULEFILEHEAD sFileHead;
	if (NULL == (bReturn = ReadFile(hAndle, (LPVOID)&sFileHead, sizeof(SSHADERMODULEFILEHEAD),
		&uByte, NULL)))
	{
		CloseHandle(hAndle);
		return E_FAIL;
	}

	if (NULL != strcmp(FILEHEADTAG, sFileHead.s_szHead))
	{
		CloseHandle(hAndle);
		return E_FAIL;
	}

	// ------------------ 获取资源个数 -----------------
	m_iResNum = sFileHead.s_iResourceFileNum;
	if (NULL == (m_pResourceInfo = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
	// ----------------- 读取资源信息 ------------------
	if (NULL == (bReturn = ReadFile(hAndle, (LPVOID)m_pResourceInfo, sizeof(SRESOURCEINFO) * m_iResNum,
		&uByte, NULL)))
	{
		CloseHandle(hAndle);
		return E_FAIL;
	}
	// 
	CloseHandle(hAndle);
	return S_OK;
}

//----------------------------------------------------------------------------
// GetResourceName
// 功能：利用资源类型索引出文件名
//----------------------------------------------------------------------------
TCHAR* CRenderObject::GetResourceName(RESTYPE enuResType)
{
	TCHAR* szReturn = NULL;
	for (int i = 0; i < m_iResNum; i++)
	{
		if (enuResType == m_pResourceInfo[i].s_enuResType)
		{
			szReturn = m_pResourceInfo[i].s_szFileName;
		}
	}
	return szReturn;
}

void CRenderObject::SetLightParament(D3DXVECTOR4* vPosition)
{
	HRESULT hr;
	hr = m_pEffect->SetVector("g_vLightPosition", vPosition);

}

VOID CRenderObject::SetConstantTexture(LPDIRECT3DTEXTURE9 tex)
{
	m_pEffect->SetTexture("texture0", tex);
}
//{{for 投影纹理
void CRenderObject::SetProjectMatrix( D3DXMATRIX matWorld, const D3DXMATRIX& matView, const D3DXMATRIX &matProject)
{
	m_pEffect->SetMatrix("g_matProjectWorld", &matWorld);
	m_pEffect->SetMatrix("g_matProjectView", &matView);
	m_pEffect->SetMatrix("g_matProjectProject", &matProject);
}
//End}}

//{{ for 绘制立方体纹理，因为每一面的视角矩阵需单独设置
void CRenderObject::SetViewMatrix(const D3DXMATRIX& matView)
{
	m_pEffect->SetMatrix("g_matView", &matView);
}
//}}

//{{ For 深度纹理图绘制
void CRenderObject::SetShadowMap(LPDIRECT3DTEXTURE9 shadowmap)
{
	m_pEffect->SetTexture("g_ShadowMap", shadowmap);
}

void CRenderObject::SetLightMatrix(const D3DXMATRIXA16 &matWorld, const D3DXMATRIXA16& matView, const D3DXMATRIXA16& matProject)
{
	m_pEffect->SetMatrix("g_matLightWorld", &matWorld);
	m_pEffect->SetMatrix("g_matLightView", &matView);
	m_pEffect->SetMatrix("g_matLightProject", &matProject);
}

void CRenderObject::SetProjectDistance(float fNear, float fFar)
{
	m_pEffect->SetFloat("g_fNear", fNear);
	m_pEffect->SetFloat("g_fFar", fFar);
}
//}}