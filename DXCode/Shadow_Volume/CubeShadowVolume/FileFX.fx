//--------------------------------------------------------------------------------------
// File: 受光物体.fx
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;
float4   g_LightPosition;

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;  
    float3 Normal     : NORMAL;
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
  };


VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    float4x4 matViewProject = mul( g_matView, g_matProject);
    Out.Position = mul( In.Position , matWorldViewProject );
    float4 v4LightPosition = mul(g_LightPosition, matViewProject);
    float4 v4Light = v4LightPosition - Out.Position;
    float4 v4Normal =normalize(mul(In.Normal, matWorldViewProject));
    if(dot(v4Light, v4Normal) < 0)
    {
    	Out.Position -= v4Light * 10;
    }
    
    
    
    return Out;
  }

float4 RenderScenePS( VS_OUTPUT In ):COLOR0
  {
    return float4(0.0, 0.0, 1.0, 1.0);
  }
//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
      	//关闭深度缓存写入
      	ZWriteEnable = false;
      	//屏蔽颜色缓存输入
      	AlPhaBlendEnable = true;
      	SrcBlend = Zero;
      	DestBlend = One;
      	//模板缓存开启
      	TwoSidedStencilMode = true;
      	CullMode     = None; //一定要开启这个才有ccw模板计算
      	StencilEnable = true;
      	StencilRef = 1;
      	StencilMask = 0xFFFFFFFF;
      	StencilWriteMask = 0xFFFFFFFF;
      	Ccw_StencilFunc = Always;
      	Ccw_StencilZFail = Incr;
      	Ccw_StencilPass = Keep;
      	StencilFunc = Always;
      	StencilZFail = Decr;
      	StencilPass = Keep;
        //FillMode     = WireFrame;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
