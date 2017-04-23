//--------------------------------------------------------------------------------------
// File: ͶӰ����
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh
texture texture0;

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };
sampler ConstantTextureSampler = sampler_state
{
	Texture = <texture0>;
	MipFilter = LINEAR;
  MinFilter = LINEAR;
  MagFilter = LINEAR;
};
float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matProjectWorld;
float4x4 g_matProjectView;
float4x4 g_matProjectProject;

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
    float2 Texcoord   : TEXCOORD;
    float4 ProjUV     : TEXCOORD2;
  };


VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Position = mul( In.Position , matWorldViewProject );
    
    Out.Texcoord = In.Texcoord;
    
    //ͶӰ����任����ʵ���ǽ�����ת��Ϊ��Ļ���꣬-1 - 1�� ��������������
    float4x4 matProjectWorldView = mul(g_matProjectWorld, g_matProjectView);
    float4x4 matProjectWorldViewProject = mul(matProjectWorldView, g_matProjectProject);
    Out.ProjUV = mul(In.Position, matProjectWorldViewProject);
    Out.ProjUV.y = -Out.ProjUV.y;
    return Out;
  }

float4 RenderScenePS( VS_OUTPUT In ):COLOR0
  {
    float4 baseColor = tex2D(MeshTextureSampler, In.Texcoord);
 		float4x4 matOffset;//��������ɫ�����ɵ�UV��-1��1�ķ�Χ����0-1�ķ�Χ��֮������������ɫ����Ӧ����Ϊ�˼��ٲ����Ķ�������
 		matOffset._11 = matOffset._22 = matOffset._33 = 0.5; //��С
    matOffset._41 = matOffset._42 = matOffset._43 = 0.5; //ƽ��
    //
    matOffset._12 = matOffset._13 = matOffset._14 = 0.0;
    matOffset._21 = matOffset._23 = matOffset._24 = 0.0;
    matOffset._31 = matOffset._32 = matOffset._34 = 0.0;
    matOffset._44 = 1.0;
    float4 Offset = mul( In.ProjUV , matOffset );
    if(Offset.x > Offset.w  || Offset.x < 0 || Offset.y > Offset.w || Offset.y < 0)//���˵�ͶӰ��ĵ�,������ͶӰ�任��û�г������ֵ��
    {
    	return baseColor;
    }
		float4 vColor = tex2Dproj( ConstantTextureSampler ,Offset );
   	return vColor * 0.7 + baseColor * 0.3;
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
