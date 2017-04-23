// ----------------------------------------------------------------------------------------------
// Ч���ļ���������Ӱ����ʵֻ������Ļ�л��������ɫ����
// ----------------------------------------------------------------------------------------------


texture texColorTexture;

float4   g_v4Position;//����
float    g_fTime ;
// ------------------------------- ���붥�����ɫ�� alpha �ṹ -----------------------------------

struct VS_INPUT
  {
    float4 pos    : POSITION;
    float4 color  : COLOR0;
    float2 tex    : TEXCOORD0;
  };
// 
struct VS_OUTPUT
  {
    float4 pos     : POSITION;
    float4 color   : COLOR0;
    float2 tex     : TEXCOORD0;
  };

sampler ColorMapSampler = sampler_state
  {
    Texture   = ( texColorTexture );
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = MIRROR;
    ADDRESSV  = MIRROR;
  };

//--------------------------------- ScreenShader --------------------------------------
//
VS_OUTPUT MainVS_Screen( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    
    //��������λ��
    
    In.pos.x = sign( In.pos.x ) * 1;
    In.pos.y = sign( In.pos.y ) * 1;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.1, 1 );
    Out.tex   = In.tex;
    Out.color = In.color;
    return Out;
  }

// -------------- ps ---------------------
float4 MainPS_Screen( VS_OUTPUT In ):COLOR
  {
			return float4(0.0, 0.0, 0.0, 0.8);
  }
  
//--------------------------- ����---------------------------

technique Default
  {
    pass P0
      {
        
        AlphaBlendEnable  = true;
        SrcBlend          = SrcAlpha;//Color;//SrcColor;//One;
        DestBlend         = InvSrcAlpha;//InvDestColor;//Zero;
        //BlendOp           = Add;
        
        StencilEnable     = true;
        StencilRef = 0;
        StencilMask = 0xFFFFFFFF;
        StencilWriteMask = 0xFFFFFFFF;
        StencilFunc       = Less;
        StencilPass       = Keep;
        
        //FillMode          = WireFrame;
        VertexShader = compile vs_2_0 MainVS_Screen();
        PixelShader  = compile ps_2_0 MainPS_Screen();
      } 
  }

