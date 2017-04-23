#define DLL_IMPLEMENT
#include "CRenderTexture.h"

CRenderTexture::CRenderTexture()
	: m_pTexture(NULL)
	, m_pDepthStencil(NULL)
	, m_pSurface(NULL)
{
}

CRenderTexture::~CRenderTexture()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pDepthStencil);
	SAFE_RELEASE(m_pSurface);
}

HRESULT CRenderTexture::OnCreateDevice(IDirect3DDevice9* pd3dDevice, INT iTextureWidth, INT iTextureHeight)
{
	HRESULT hr;
	//´´½¨ÎÆÀí
	HR(D3DXCreateTexture(pd3dDevice, iTextureWidth, iTextureHeight, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture));
	HR(pd3dDevice->CreateDepthStencilSurface(iTextureWidth, iTextureHeight, D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE, 0, TRUE, &m_pDepthStencil, NULL));
	HR(m_pTexture->GetSurfaceLevel(0, &m_pSurface));
	return S_OK;
}

void CRenderTexture::RenderTexture(IDirect3DDevice9* pd3dDevice, void(*RenderObjects)())
{
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	LPDIRECT3DSURFACE9 pBackDepthStencil = NULL;

	pd3dDevice->GetRenderTarget(0, &pBackBuffer);
	pd3dDevice->GetDepthStencilSurface(&pBackDepthStencil);
	pd3dDevice->SetRenderTarget(0, m_pSurface);
	pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0000ffff, 1.0f, 0);
	pd3dDevice->BeginScene();
	RenderObjects();
	pd3dDevice->EndScene();

	pd3dDevice->SetRenderTarget(0, pBackBuffer);
	SAFE_RELEASE(pBackBuffer);
	pd3dDevice->SetDepthStencilSurface(pBackDepthStencil);
	SAFE_RELEASE(pBackDepthStencil);
}
