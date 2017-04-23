#ifndef __SIMPLE_PARTICLE_H__
#define __SIMPLE_PARTICLE_H__
#include"Target.h"
#pragma comment(lib, "winmm.lib")

class CSimpleParticle :public IParticle
{
public:
	CSimpleParticle();
	~CSimpleParticle();
	virtual BOOL Init(IDirect3DDevice9* pDevice);
	virtual VOID Release();
	virtual INT GetVertexSize() { return sizeof(STParticle); };
	virtual INT GetVertexCount() {
		return m_VertexCount;
	};
	virtual INT GetFaceCount() {
		return m_IndexCount / 3;
	};
	virtual LPDIRECT3DVERTEXBUFFER9 GetVertexBuffer() {
		return m_pVB;
	};
	virtual LPDIRECT3DVERTEXDECLARATION9 GetVertexDeclaration() {
		return m_VD;
	};
	virtual LPDIRECT3DINDEXBUFFER9 GetIndexBuffer() {
		return m_pIB;
	};
private:
	struct STParticle
	{
		float x, y, z, w;
		float u, v;
	};
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9 m_pIB;
	INT m_VertexCount;
	INT m_IndexCount;
	LPDIRECT3DVERTEXDECLARATION9 m_VD;
};

class CSimpleParticleState : public IParticleState
{
public:
	virtual void UpdateState(float fTime);
	virtual BOOL Init(INT ParticleId);
	virtual const D3DXVECTOR4& GetPosition() {
		return m_Position;
	};
	virtual BOOL IsVisible() {
		return m_bVisible;
	};
	virtual VOID Release();

private:
	static const float m_AGravity;
	D3DXVECTOR4 m_Position;
	D3DXVECTOR4 m_ASpeed;
	D3DXVECTOR4 m_Speed;
	BOOL m_bVisible;
	FLOAT m_delay;
};

struct STSimpleParticleParam
{
	D3DXMATRIXA16* pWorld;
	D3DXMATRIXA16* pView;
	D3DXMATRIXA16* pProject;
};
class CSimpleParticleEffect :public IParticleEffect
{
public:
	virtual BOOL Init(LPDIRECT3DDEVICE9 pDevice);
	virtual LPD3DXEFFECT GetEffect() {
		return m_pEffect;
	};
	virtual VOID Update(float fTime, void* Param, IParticleState** ParticleStateArray, INT ParticleCount);
	virtual VOID Release() { /*Ê¡ÂÔ*/ };
private:
	LPD3DXEFFECT m_pEffect;
};

typedef TParticleSystem<CSimpleParticle, CSimpleParticleState, CSimpleParticleEffect> CSimpleParticleSystem;

#endif