
#include "Engine_Shader_Defines.hlsli"

float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;

float4			g_vCamPosition;

struct VS_IN
{
	/* 그리기용 정점데이터 m_pVB */
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	/* 변환용 정점데이터 m_pVBInstance */
	row_major matrix	TransformMatrix : WORLD;
	
	float2		vLifeTime : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
	float2		vLifeTime : TEXCOORD0;
};

/* 언제 수행이되느냐? : 정점하나당 각자 수행.  */
VS_OUT VS_MAIN(VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	

	matrix			matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/* 그리기용 정점의 데이터를 로컬스페이스 내에서 TransformMatrix상태로 변환한다. */
	vector	vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);	

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = float2(In.vPSize.x * length(In.TransformMatrix._11_12_13), In.vPSize.y * length(In.TransformMatrix._21_22_23));
	Out.vLifeTime = In.vLifeTime;

	return Out;		
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
	float2		vLifeTime : TEXCOORD0;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float2		vLifeTime : TEXCOORD1;
};

/* 언제 수행이되느냐? : 도형하나당 한번씩 수행. */
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> VertexStream)
{
	GS_OUT			Out[4];

	float4			vLook = g_vCamPosition - In[0].vPosition;
	float4			vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f, 0.f);
	float4			vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)) * In[0].vPSize.y * 0.5f, 0.f);

	matrix			matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = In[0].vPosition + vRight + vUp;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vLifeTime = In[0].vLifeTime;

	Out[1].vPosition = In[0].vPosition - vRight + vUp;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);
	Out[1].vLifeTime = In[0].vLifeTime;

	Out[2].vPosition = In[0].vPosition - vRight - vUp;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vLifeTime = In[0].vLifeTime;

	Out[3].vPosition = In[0].vPosition + vRight - vUp;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vLifeTime = In[0].vLifeTime;

	VertexStream.Append(Out[0]);
	VertexStream.Append(Out[1]);
	VertexStream.Append(Out[2]);
	VertexStream.RestartStrip();

	VertexStream.Append(Out[0]);
	VertexStream.Append(Out[2]);
	VertexStream.Append(Out[3]);
	VertexStream.RestartStrip();
}




struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float2		vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

/* 언제 수행이되느냐? : 픽셀하나당 한번씩 수행. */
/* 픽셀의 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);	
	
	if (Out.vColor.a <= 0.1f)
		discard;

	Out.vColor = 1.f;

	return Out;
}

technique11			DefaultTechnique
{

	pass Default
	{		
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

