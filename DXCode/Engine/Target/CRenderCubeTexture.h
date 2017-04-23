//******************************************************
// CreateBy Mailko
// Date 2016-12-09
// For ����һ���ɹ����Ƶ�������������֧�ֻ��Ƹ�����
//******************************************************
#ifndef _CRENDER_CUBE_TEXTURE_
#define _CRENDER_CUBE_TEXTURE_
#include <d3d9.h>
#include <d3dx9.h>
#include "Common.h"

class DLL_API CRenderCubeTexture
{
private:
	INT m_TextureSize;// =256
	IDirect3DCubeTexture9* m_pCubeTexture;
	LPDIRECT3DSURFACE9 m_pDepthStencil;

	D3DXMATRIX m_matView[6];//�������������������ӽǾ���

public:
	CRenderCubeTexture();
	~CRenderCubeTexture();
	IDirect3DCubeTexture9* GetTexture() { return m_pCubeTexture; }
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice);
	void RenderCube(IDirect3DDevice9* pd3dDevice, void (*RenderObjects)(const D3DXMATRIX& matView));

};
#endif