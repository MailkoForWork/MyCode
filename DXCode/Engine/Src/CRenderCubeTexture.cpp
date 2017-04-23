#define DLL_IMPLEMENT
#include "CRenderCubeTexture.h"

CRenderCubeTexture::CRenderCubeTexture()
	: m_pCubeTexture(NULL)
	, m_pDepthStencil(NULL)
	, m_TextureSize(256)
{
	for (int iMat = 0; iMat < 6; iMat++)
	{
		D3DXMatrixIdentity(&m_matView[iMat]);
	}
	m_matView[D3DCUBEMAP_FACE_POSITIVE_X] = D3DXMATRIX(
		0, 0, 1, 0,
		0, 1, 0, 0,
		-1, 0, 0, 0,
		0, 0, 0, 1
		);

	m_matView[D3DCUBEMAP_FACE_NEGATIVE_X] = D3DXMATRIX(
		0, 0, -1, 0,
		0, 1, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, 1
		);

	m_matView[D3DCUBEMAP_FACE_POSITIVE_Y] = D3DXMATRIX(
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, -1, 0, 0,
		0, 0, 0, 1
		);

	m_matView[D3DCUBEMAP_FACE_NEGATIVE_Y] = D3DXMATRIX(
		1, 0, 0, 0,
		0, 0,-1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
		);
	m_matView[D3DCUBEMAP_FACE_POSITIVE_Z] = D3DXMATRIX(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		);
	m_matView[D3DCUBEMAP_FACE_NEGATIVE_Z] = D3DXMATRIX(
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1
		);


}

CRenderCubeTexture::~CRenderCubeTexture()
{
	SAFE_RELEASE(m_pCubeTexture);
	SAFE_RELEASE(m_pDepthStencil);
}

HRESULT CRenderCubeTexture::OnCreateDevice(IDirect3DDevice9* pd3dDevice)
{
	HRESULT hr;
	HR(D3DXCreateCubeTexture(pd3dDevice, m_TextureSize, 1,
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pCubeTexture));

	HR(pd3dDevice->CreateDepthStencilSurface(m_TextureSize, m_TextureSize, D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE, 0, TRUE, &m_pDepthStencil, NULL))
	OutputDebugString(TEXT("CRenderCubeTexture Create Success"));
	return S_OK;
}

VOID CRenderCubeTexture::RenderCube(IDirect3DDevice9* pd3dDevice, void(*RenderObjects)(const D3DXMATRIX& matView))
{
	HRESULT hr;
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	hr = pd3dDevice->GetRenderTarget(0, &pBackBuffer);
	LPDIRECT3DSURFACE9 pBackDepthStencil = NULL;
	hr = pd3dDevice->GetDepthStencilSurface(&pBackDepthStencil);
	pd3dDevice->SetDepthStencilSurface(m_pDepthStencil);

	//¿ªÊ¼»æÖÆ
	for (int iSurf = 0; iSurf < 6; iSurf++)
	{
		LPDIRECT3DSURFACE9 pSurf = NULL;
		hr = m_pCubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)iSurf, 0, &pSurf);
		hr = pd3dDevice->SetRenderTarget(0, pSurf);
		SAFE_RELEASE(pSurf);
		hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0000ffff, 1.0f, 0);
		pd3dDevice->BeginScene();
		RenderObjects(m_matView[iSurf]);
		pd3dDevice->EndScene();
	}
	
	pd3dDevice->SetDepthStencilSurface(pBackDepthStencil);
	hr = pd3dDevice->SetRenderTarget(0, pBackBuffer);
	SAFE_RELEASE(pBackBuffer);
}