
#include "Engine_Shader_Defines.hlsli"

float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector			g_vColor;

struct VS_IN
{
	float3		vPosition : POSITION;	
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
};

VS_OUT VS_MAIN(VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	

	matrix			matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	return Out;		
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

/* 픽셀의 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;	

	Out.vColor = g_vColor;

	return Out;
}

struct GS_IN
{
    float4 vPosition : SV_Position;
};
struct GS_OUT
{
    float4 vPosition : SV_Position;
};

[maxvertexcount(4)]
void GS_Main(line GS_IN Input[2], inout TriangleStream<GS_OUT> triStream)
{
    GS_OUT Output = (GS_OUT) 0;
    float fThickness = 0.2f;
    float2 vDirection = normalize(Input[1].vPosition.xy - Input[0].vPosition.xy);
    float2 vPerpendicular = float2(-vDirection.y, vDirection.x) * fThickness * 0.5;
    
    Output.vPosition = Input[0].vPosition + float4(vPerpendicular, 0, 0);
    triStream.Append(Output);
    
    Output.vPosition = Input[1].vPosition + float4(vPerpendicular, 0, 0);
    triStream.Append(Output);
    
    Output.vPosition = Input[0].vPosition - float4(vPerpendicular, 0, 0);
    triStream.Append(Output);
    
    Output.vPosition = Input[1].vPosition - float4(vPerpendicular, 0, 0);
    triStream.Append(Output);
    
    triStream.RestartStrip();
}


technique11			DefaultTechnique
{
	/* Pass : 셰이더 기능의 캡슐화 (여러기법을 모아놓은) */

	/* 빛연산 + 림라이트 + 그림자 + 메탈릭 */
	pass Default
	{		
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

