//--------------------------------------------------------------------------------------
// File: ������ϵͳ
// 
//
//--------------------------------------------------------------------------------------

texture g_ParticleTex;//��������Particle.bmp�ļ�
sampler ParticleTexSampler = sampler_state
{
		Texture = <g_ParticleTex>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

float4x4 g_matWorld;
float4x4 g_matView;
float4x4 g_matProject;

static const float ParticleOffset = 0.02;

#define PARTICLE_COUNT 250
float4 g_ParticleArray[PARTICLE_COUNT];

struct VS_INPUT  //��ӦSimpleParticle.h�е�STParticle�ṹ �� ���������ʶ
{
	float4 Position		:	POSITION;
	float2 uv				  : TEXCOORD0;
	float  Index      : TEXCOORD1;
};

struct VS_OUTPUT
{
	float4 Position		: POSITION;
	float2 Texcoord   : TEXCOORD0;
};

VS_OUTPUT ParticleVS(VS_INPUT Input)
{
	float4 centerPosition = g_ParticleArray[Input.Index];
	float4x4 matWorldView = mul( g_matWorld ,g_matView );
  float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
  centerPosition = mul(centerPosition, matWorldViewProject);
  centerPosition = centerPosition / centerPosition.w;
  VS_OUTPUT Out;
  Out.Position = centerPosition;
  Out.Position.x = sign(Input.Position.x) * ParticleOffset + centerPosition.x;
  Out.Position.y = sign(Input.Position.y) * ParticleOffset + centerPosition.y;
  Out.Texcoord = Input.uv;
  return Out;
}

float4 ParticlePS(VS_OUTPUT Input):COLOR0
{
	return tex2D(ParticleTexSampler, Input.Texcoord);
}

technique DefaultTech
{
	pass P0
	{
	   // ͸��
	      ZEnAble             = true;
        ZWriteEnable        = false;
        AlphaBlendEnable  = true;
        SrcBlend          = SrcAlpha;//SrcAlpha;
        DestBlend         = One;//InvSrcAlpha;//One;//DestAlpha
        
		VertexShader = compile vs_2_0 ParticleVS();
		PixelShader = compile ps_2_0 ParticlePS();
	}
}