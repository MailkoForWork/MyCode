#define DLL_IMPLEMENT
#include"CBoard.h"

CBoard::CBoard()
	: m_Tex(NULL)
	, m_Vertexs(NULL)
	, m_Indexs(NULL)
	, m_NumVertex(0)
	, m_NumIndex(0)
	, m_Desc(0)
	, m_Effect(0)
{

}

CBoard::~CBoard()
{

}

VOID CBoard::Init()
{

}

HRESULT CBoard::OnCreateDevice(IDirect3DDevice9* pDevice)
{
	HRESULT hr = S_OK;
	HR(D3DXCreateTextureFromFile(pDevice, "ColorMap.dds", &m_Tex));
	HR(CreateSquare(pDevice));

	//创建顶点描述
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
		{ 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	if (FAILED(hr = pDevice->CreateVertexDeclaration(decl, &m_Desc))) return hr;

	//创建着色器效果
	LPD3DXBUFFER pCode = NULL;
	if (FAILED(hr = D3DXCreateEffectFromFile(pDevice, TEXT("FileFX.fx"), NULL, NULL, 0, NULL, &m_Effect, &pCode)))
	{
		LPVOID pBufErr = pCode->GetBufferPointer();
		char * Err = (char*)pBufErr;
		MessageBox(NULL, Err, TEXT("着色器编译错误"), 0);
		return hr;
	}

	m_Effect->SetTexture("texColorTexture", m_Tex);

	return S_OK;
}

HRESULT CBoard::CreateSquare(IDirect3DDevice9 *pDevice)
{
	m_NumVertex = 4;
	m_NumIndex = 4;
	HRESULT hr = S_OK;

	//创建索引缓存
	HR(pDevice->CreateIndexBuffer(sizeof(WORD) * m_NumIndex, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_Indexs, NULL));
	WORD* pIndexs;
	HR(m_Indexs->Lock(0, 0, (void**)&pIndexs, 0));
	pIndexs[0] = 0;
	pIndexs[1] = 1;
	pIndexs[2] = 2;
	pIndexs[3] = 3;

	HR(m_Indexs->Unlock());

	//创建顶点缓存
	BOARD_D3DVERTEX *pVertices = NULL;
	HR(pDevice->CreateVertexBuffer(sizeof(BOARD_D3DVERTEX) * m_NumVertex, 0, 0, D3DPOOL_DEFAULT, &m_Vertexs, NULL));
	HR(m_Vertexs->Lock(0, 0, (void**)&pVertices, 0));
	// 在 X Z 平面
	// 红
	pVertices[0].x = -10;
	pVertices[0].y = 10;
	pVertices[0].z = 0;
	pVertices[0].w = 1;
	pVertices[0].color = D3DCOLOR_ARGB(0x70, 0xFF, 0, 0);  // 透明顶点
	pVertices[0].u = 0;
	pVertices[0].v = 0;
	// 绿
	pVertices[1].x = 10;
	pVertices[1].y = 10;
	pVertices[1].z = 0;
	pVertices[1].w = 1;
	pVertices[1].color = D3DCOLOR_ARGB(0x70, 0x0, 0xFF, 0);  // 透明顶点
	pVertices[1].u = 1;
	pVertices[1].v = 0;
	// 蓝
	pVertices[2].x = -10;
	pVertices[2].y = -10;
	pVertices[2].z = 0;
	pVertices[2].w = 1;
	pVertices[2].color = D3DCOLOR_ARGB(0x70, 0x0, 0, 0xFF);
	pVertices[2].u = 0;
	pVertices[2].v = 1;
	// 白
	pVertices[3].x = 10;
	pVertices[3].y = -10;
	pVertices[3].z = 0;
	pVertices[3].w = 1;
	pVertices[3].color = D3DCOLOR_ARGB(0x70, 0x0FF, 0xFF, 0);
	pVertices[3].u = 1;
	pVertices[3].v = 1;
	HR(m_Vertexs->Unlock());

	return S_OK;
}

HRESULT CBoard::OnResetDevice(IDirect3DDevice9* pDevice)
{
	return S_OK;
}

void CBoard::OnFrameMove(float fTime, D3DXVECTOR4 position)
{
	HRESULT hr = S_OK;
	hr = m_Effect->SetFloat("g_fTime", fTime);
	hr = m_Effect->SetVector("g_v4Position", &position);
}

void CBoard::OnFrameRender(IDirect3DDevice9* pd3dDevice)
{
	HRESULT hr = S_OK;
	hr = pd3dDevice->SetVertexDeclaration(m_Desc);
	hr = pd3dDevice->SetStreamSource(0, m_Vertexs, 0, sizeof(BOARD_D3DVERTEX));
	hr = pd3dDevice->SetIndices(m_Indexs);
	UINT iPass, cPasses;

	hr = m_Effect->SetTechnique("Default");
	hr = m_Effect->Begin(&cPasses, 0);
	for (iPass = 0; iPass < cPasses; iPass++)
	{
		hr = m_Effect->BeginPass(iPass);
		hr = pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_NumVertex, 0, 2);
		hr = m_Effect->EndPass();
	}
	hr = m_Effect->End();
}

void CBoard::OnDestroyDevice()
{
	SAFE_RELEASE(m_Tex);
	SAFE_RELEASE(m_Vertexs);
	SAFE_RELEASE(m_Indexs);
	SAFE_RELEASE(m_Desc);
	SAFE_RELEASE(m_Effect);
}

void CBoard::SetTranslateMatrix(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject)
{
	HRESULT hr = S_OK;
	hr = m_Effect->SetMatrix("g_matWorld", pWorld);
	hr = m_Effect->SetMatrix("g_matView", pView);
	hr = m_Effect->SetMatrix("g_matProject", pProject);
}

void CBoard::ChangeTexture(LPDIRECT3DTEXTURE9 pTexture)
{
	HRESULT hr = S_OK;
	hr = m_Effect->SetTexture("texColorTexture", pTexture);
	if (FAILED(hr))
	{
		OutputDebugString("Wrong");
	}
}

VOID CBoard::SetTextureSize(INT Width, INT Height)
{
	INT Size[2] = { Width, Height };
	m_Effect->SetIntArray("iTexSize", Size, 2);
}

VOID CBoard::SetMosaicSize(INT Width, INT Height)
{
	INT Size[2] = { Width, Height };
	m_Effect->SetIntArray("iMosaicSize", Size, 2);
}
