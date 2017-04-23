//******************************************************
// CreateBy Mailko
// Date 2016-12-10
// For 创建一个可供绘制的纹理。并支持绘制该纹理
//******************************************************
#ifndef _CRENDER_TEXTURE_
#define _CRENDER_TEXTURE_
#include <d3d9.h>
#include <d3dx9.h>
#include "Common.h"

class DLL_API CRenderTexture
{
private:
	IDirect3DTexture9* m_pTexture;
	IDirect3DSurface9* m_pSurface;
	LPDIRECT3DSURFACE9 m_pDepthStencil;


public:
	CRenderTexture();
	~CRenderTexture();
	IDirect3DTexture9* GetTexture() { return m_pTexture; }
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice, INT TextureWidth, INT TextureHeight);
	void RenderTexture(IDirect3DDevice9* pd3dDevice, void(*RenderObjects)());
};
#endif