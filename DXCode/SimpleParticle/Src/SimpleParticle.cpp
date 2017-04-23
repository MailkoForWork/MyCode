#include"../Inc/SimpleParticle.h"
const FLOAT CSimpleParticleState::m_AGravity = 0.98;
CSimpleParticle::CSimpleParticle()
{

}

CSimpleParticle::~CSimpleParticle()
{

}

BOOL CSimpleParticle::Init(IDirect3DDevice9* pDevice)
{
	HRESULT hr = S_OK;
	m_VertexCount = 4;
	m_IndexCount = 6;

	//构建顶点描述：
	D3DVERTEXELEMENT9 decl[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{1, 0, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		D3DDECL_END()
	};
	HR(pDevice->CreateVertexDeclaration(decl, &m_VD));

	//构建顶点缓存
	HR(pDevice->CreateVertexBuffer(sizeof(STParticle) * m_VertexCount, 0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL));
	STParticle* pVertices = NULL;
	HR(m_pVB->Lock(0, 0, (void**)&pVertices, 0));
	pVertices[0].x = -10;
	pVertices[0].y = 10;
	pVertices[0].z = 0;
	pVertices[0].w = 1;
	pVertices[0].u = 0;
	pVertices[0].v = 1;

	pVertices[1].x = 10;
	pVertices[1].y = 10;
	pVertices[1].z = 0;
	pVertices[1].w = 1;
	pVertices[1].u = 1;
	pVertices[1].v = 1;

	pVertices[2].x = 10;
	pVertices[2].y = -10;
	pVertices[2].z = 0;
	pVertices[2].w = 1;
	pVertices[2].u = 1;
	pVertices[2].v = 0;

	pVertices[3].x = -10;
	pVertices[3].y = -10;
	pVertices[3].z = 0;
	pVertices[3].w = 1;
	pVertices[3].u = 0;
	pVertices[3].v = 0;
	HR(m_pVB->Unlock());

	//构建索引缓存
	HR(pDevice->CreateIndexBuffer(sizeof(WORD) * m_IndexCount, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL));
	WORD* pIndex;
	HR(m_pIB->Lock(0, 0, (VOID**)&pIndex, 0));
	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 2;
	pIndex[3] = 0;
	pIndex[4] = 2;
	pIndex[5] = 3;
	HR(m_pIB->Unlock());

	return TRUE;
}

VOID CSimpleParticle::Release()
{
	//待完善
}

void CSimpleParticleState::UpdateState(float fTime)
{
	m_delay -= fTime;
	if (m_delay > 0)
	{
		return;
	}
	D3DXVECTOR4 SpeedBefore;
	SpeedBefore = m_Speed;
	m_Speed = SpeedBefore + fTime * m_ASpeed;
	D3DXVECTOR4 DPosition;
	DPosition = fTime * m_Speed + m_ASpeed * fTime * fTime/ 2;
	m_Position += DPosition;
	if (m_delay < -2.0f)
	{
		Init(0);
	}
}

BOOL CSimpleParticleState::Init(INT ParticleId)
{
	//srand(::timeGetTime());
	m_Position = D3DXVECTOR4(0, 8, 0, 1);
	//m_Position.y = (600 + rand() % 501) / 100.0f; //6~11范围，两位小数

	//给一个初速度
	m_Speed.y = 30;
	m_Speed.z = (-800 + rand() % 1601) / 100.0f;//-1 ~ 1 范围，两位小数
	m_Speed.x = (-800 + rand() % 1601) / 100.0f;
	m_Speed.w = 0;
	//初始化加速度
	m_ASpeed.y = - 60.0f;
	m_ASpeed.x = 0/*SPEED_TEST / ACCELERATION_TIME*/;
	m_ASpeed.z = 0/*SPEED_TEST / ACCELERATION_TIME*/;
	m_ASpeed.w = 0;

	m_bVisible = TRUE;
	if(ParticleId != 0)
	{
		m_delay = (rand() % 201) / 100.0f;
	}
	else
	{
		m_delay = 0;
	}
	return TRUE;
}

VOID CSimpleParticleState::Release()
{
	//省略
}

BOOL CSimpleParticleEffect::Init(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT hr;
	LPD3DXBUFFER pCode = NULL;
	if (FAILED(hr = D3DXCreateEffectFromFile(pDevice, TEXT("Particle.fx"), NULL, NULL, 0, NULL, &m_pEffect, &pCode)))
	{
		LPVOID pBuffer = pCode->GetBufferPointer();
		char* ERR = (char*)pBuffer;
		MessageBox(NULL, ERR, TEXT("着色器编译错误"), 0);
		return hr;
	}
	LPDIRECT3DTEXTURE9 pTex = NULL;
	//HR(D3DXCreateTextureFromFile(pDevice, "ColorMap.dds", &pTex));
	HR(D3DXCreateTextureFromFileEx(pDevice, "ColorMap.dds", D3DX_DEFAULT, D3DX_DEFAULT,
		D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0,
		NULL, NULL, &pTex));
	m_pEffect->SetTexture("g_ParticleTex", pTex);
}

void CSimpleParticleEffect::Update(float fTime, void* pParam, IParticleState** ParticleArray, INT ParticleCount)
{
	STSimpleParticleParam* pParticleParam = (STSimpleParticleParam*)pParam;
	HRESULT hr = S_OK;
	hr = m_pEffect->SetMatrix("g_matWorld", pParticleParam->pWorld);
	hr = m_pEffect->SetMatrix("g_matView", pParticleParam->pView);
	hr = m_pEffect->SetMatrix("g_matProject", pParticleParam->pProject);
	static D3DXVECTOR4* vtParticleArray = NULL;
	if (NULL == vtParticleArray)
	{
		vtParticleArray =(D3DXVECTOR4*) malloc(sizeof(D3DXVECTOR4) * ParticleCount);
	}
	for (int i = 0; i < ParticleCount; i++)
	{
		CSimpleParticleState* pState = (CSimpleParticleState*)ParticleArray[i];
		vtParticleArray[i] = pState->GetPosition();
	}
	hr = m_pEffect->SetVectorArray("g_ParticleArray", vtParticleArray, ParticleCount);
}

