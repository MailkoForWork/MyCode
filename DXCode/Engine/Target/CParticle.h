//----------------------------------------------
//CreateBy Mailko
//CreateFor 粒子实现效果
//解析：主要为三个主要抽象组件
// 粒子实例：存储每个粒子的基本常量比如顶点缓存
// 粒子状态：单个粒子的状态，通过id来辨别
// 粒子系统：模板类，将上两者联系在一起，从而创建绘制释放
//----------------------------------------------

#ifndef _CPARTICLE_H_
#define _CPARTICLE_H_
#include "Common.h"

struct STParticleFlag
{
	FLOAT Index;
};
interface IParticleState
{
public:
	virtual void UpdateState(float fTime) = 0;
	virtual BOOL Init(INT ParticleId) = 0;//为了一些粒子初始化添加的参数
	virtual	BOOL IsVisible() = 0;
	virtual VOID Release() = 0;
};

interface IParticle
{
public:
	virtual BOOL Init(IDirect3DDevice9* pDevice) = 0;
	virtual VOID Release() = 0;
	virtual INT GetVertexSize() = 0;
	virtual INT GetVertexCount() = 0;
	virtual INT GetFaceCount() = 0;
	virtual LPDIRECT3DVERTEXBUFFER9 GetVertexBuffer() = 0;
	virtual LPDIRECT3DVERTEXDECLARATION9 GetVertexDeclaration() = 0;//包含状态的顶点描述
	virtual LPDIRECT3DINDEXBUFFER9 GetIndexBuffer() = 0;
};

__interface IParticleEffect
{
public:
	virtual BOOL Init(LPDIRECT3DDEVICE9 pDevice) = 0; 
	virtual LPD3DXEFFECT GetEffect() = 0;
	virtual VOID Update(float fTime, void* pParams, IParticleState** ParticleStateArray, INT ParticleCount) = 0;
	virtual void Release() = 0;
};
template<typename ParticleClass, typename ParticleStateClass, typename ParticleEffectClass>
class TParticleSystem
{
public:

	//粒子系统的创建，删除以及显示系统
	TParticleSystem()
		: m_ParticleNum(0)
		, m_pParticleInstance(NULL)
		, m_pParticleStates(NULL)
	{};
	~TParticleSystem() {};
	HRESULT OnCreateDevice(IDirect3DDevice9* pDevice, INT ParticleNum)
	{
		HRESULT hr = S_OK;
		if (ParticleNum <= 0)
		{
			OutputDebugString(TEXT("CParticle::OnCreateDevice ParticleNum is less than 0"));
			return S_FALSE;
		}
		m_ParticleNum = ParticleNum;
		if (!MallocPaticleStates(m_ParticleNum))
		{
			return S_FALSE;
		}
		for (INT ParticleIndex = 0; ParticleIndex < m_ParticleNum; ParticleIndex++)
		{
			m_pParticleStates[ParticleIndex]->Init(ParticleIndex);
		}
		m_pParticleInstance->Init(pDevice);

		pDevice->CreateVertexBuffer(m_ParticleNum * sizeof(STParticleFlag), 0, 0, D3DPOOL_MANAGED, &m_FlagBuffer, 0);
		STParticleFlag* pBuffer;
		m_FlagBuffer->Lock(0, 0, (VOID**)&pBuffer, 0);
		for (int i = 0; i < m_ParticleNum; i++)
		{
			pBuffer[i].Index = i;
		}
		m_FlagBuffer->Unlock();

		//着色器
		HR(m_pParticleEffect->Init(pDevice));
		return S_OK;
	}
	void OnFrameMove(float fTime, void* pEffectParam)
	{
		for (int i = 0; i < m_ParticleNum; i++)
		{
			m_pParticleStates[i]->UpdateState(fTime);
		}
		m_pParticleEffect->Update(fTime, pEffectParam, m_pParticleStates, m_ParticleNum);
	}
	void OnFrameRender(IDirect3DDevice9* pDevice)
	{
		HRESULT hr = S_OK;

		hr = pDevice->SetVertexDeclaration(m_pParticleInstance->GetVertexDeclaration());
		hr = pDevice->SetStreamSource(0, m_pParticleInstance->GetVertexBuffer(), 0, m_pParticleInstance->GetVertexSize());
		hr = pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | m_ParticleNum);
		hr = pDevice->SetIndices(m_pParticleInstance->GetIndexBuffer());
		hr = pDevice->SetStreamSource(1, m_FlagBuffer, 0, sizeof(STParticleFlag));
		hr = pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);

		//绘制
		UINT iPass, cPasses;
		LPD3DXEFFECT pEffect = m_pParticleEffect->GetEffect();
		if (NULL == pEffect)
		{
			return;
		}
		hr = pEffect->Begin(&cPasses, 0);
		for (iPass = 0; iPass < cPasses; iPass++)
		{
			pEffect->BeginPass(iPass);
			pEffect->CommitChanges();
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_pParticleInstance->GetVertexCount(), 0, m_pParticleInstance->GetFaceCount());
			pEffect->EndPass();
		}

		hr = pEffect->End();
		pDevice->SetStreamSourceFreq(0, 1);
		pDevice->SetStreamSourceFreq(1, 1);
	}
	void OnDestroyDevice()
	{
		for (INT ParticleIndex = 0; ParticleIndex < m_ParticleNum; ParticleIndex++)
		{
			m_pParticleStates[ParticleIndex]->Release();
		}
		SAFE_DELETE(m_pParticleStates);
		m_pParticleInstance->OnDestroyDevice();
	}
	IParticle* GetParticle() { return m_pParticleInstance; }
	IParticleState** GetStateArray() { return m_pParticleStates; }
	INT GetParticleCount() { return m_ParticleNum; }
	IParticleEffect* GetEffect() { return m_pParticleEffect; }


	//针对每一个粒子的处理工作，需要创建粒子系统时，需要实现接下来的虚函数
protected:
	virtual BOOL MallocPaticleStates(INT ParticleNum)
	{
		m_pParticleStates = (IParticleState**)malloc(sizeof(IParticleState*) * ParticleNum);
		for (int i = 0; i < ParticleNum; i++)
		{
			m_pParticleStates[i] = new ParticleStateClass;
		}
		m_pParticleInstance = new ParticleClass;
		m_pParticleEffect = new ParticleEffectClass;
		if (m_pParticleStates == NULL || m_pParticleInstance == NULL || m_pParticleEffect == NULL)
		{
			OutputDebugString(TEXT("CParticle::OnCreateDevice MallocParticles and  particleStates FAILED"));
			return FALSE;
		}
		return TRUE;
	}
private:
	INT m_ParticleNum;
	IParticleState** m_pParticleStates;//粒子的状态，
	IParticle* m_pParticleInstance;//粒子单例
	IParticleEffect* m_pParticleEffect;//粒子渲染器
	LPDIRECT3DVERTEXBUFFER9 m_FlagBuffer;
};

#endif