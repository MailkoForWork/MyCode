//--------------------------------------------------------------------------------------
// File: 纹理深度图绘制.fx
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh
texture g_ShadowMap;          //纹理深度图
float g_fNear;
float g_fFar;

float4x4 g_matLightWorld;
float4x4 g_matLightView;
float4x4 g_matLightProject;

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

sampler ShadowMapSampler = sampler_state
{
	Texture = <g_ShadowMap>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;  
    float3 Normal     : NORMAL;
    float2 Texcoord   : TEXCOORD;
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float4 ShadowMapPosition  : TEXCOORD1;
    float2 Texcoord   : TEXCOORD;
  };


VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Position = mul( In.Position , matWorldViewProject );
    
    //计算相应的ShadowMap位置参数
   	float4x4 matLightWorldView = mul(g_matLightWorld, g_matLightView);
   	float4x4 matLightWorldViewProject = mul(matLightWorldView, g_matLightProject);
   	Out.ShadowMapPosition = mul(In.Position, matLightWorldViewProject);
		
    Out.Texcoord = In.Texcoord;
    return Out;
  }

float4 RenderScenePS( VS_OUTPUT In ):COLOR0
  {
  	float ObjectZ = In.ShadowMapPosition.z / g_fFar;
  	float2 ShadowUV = (In.ShadowMapPosition.xy / In.ShadowMapPosition.w + float2(1, 1)) / 2;
  	ShadowUV.y = 1 - ShadowUV.y;
  	float ShadowMapZ = tex2D(ShadowMapSampler, ShadowUV).x;
  	float4 color = tex2D(MeshTextureSampler, In.Texcoord);
  	if(ShadowMapZ + 0.009 <= ObjectZ) //在阴影内
  	{
  		color = 0;//color * 0.2;
  	}

    return color;
  }
//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        //FillMode     = WireFrame;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
