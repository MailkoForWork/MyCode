// ----------------------------------------------------------------------------------------------
// Ч���ļ���ģ�建��д�룬д�����½ǣ�����Ķ�������ֻ�ж���λ�õķ�������
// PSЧ���������ˣ����ڴ�С 1024 * 1024
// tip ����ɫ��ѭ���������ò���
//
// ----------------------------------------------------------------------------------------------


texture texColorTexture;
int2 iTexSize;
int2 iMosaicSize;

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
    
    In.pos.x = sign( In.pos.x ) * 1.0f / 4 * 3;
    In.pos.y = sign( In.pos.y ) * 1.0f / 4 * 3;
    Out.pos  = float4( In.pos.x , In.pos.y , 0.1, 1 );
    Out.tex   = In.tex;
    Out.color = In.color;
    return Out;
  }

// -------------- ps ---------------------
float4 MainPS_Screen( VS_OUTPUT In ):COLOR
  {
  
    float2 fUVPerArea = iMosaicSize * 1.0f / iTexSize;
    int2 fUVPixel = int2(In.tex * iTexSize); //����������
    int2 Area = int2(fUVPixel.x / iMosaicSize.x, fUVPixel.y / iMosaicSize.y); //�����������˵�����
    float2 UV = (Area + 0.5) * fUVPerArea;

    float4 v4Texture = tex2D(ColorMapSampler, UV);  
    float4 v4Out = float4( v4Texture.x , v4Texture.y , v4Texture.z , 1.0 -  0.3 );
    return v4Out;
  }

float4 MainPS_Screen_Line( VS_OUTPUT In ):COLOR
  {
    return float4( 1.0 ,1.0 ,1.0 ,0.0 );
  }

//--------------------------- ����---------------------------

technique Default
  {
    pass P0
      {
        //AlphaBlendEnable = true;
        //SrcBlend = SrcAlpha;
       // DestBlend = DestColor;
        //BlendOp = Add; 
        
        //FillMode          = WireFrame;
        VertexShader = compile vs_2_0 MainVS_Screen();
        PixelShader  = compile ps_2_0 MainPS_Screen();
      }
    //pass P1
    // {
    //   AlphaBlendEnable  = false;
      //  FillMode          = WireFrame;
     //   VertexShader = compile vs_2_0 MainVS_Screen();
     //   PixelShader  = compile ps_2_0 MainPS_Screen_Line();
     // }
    
  }

