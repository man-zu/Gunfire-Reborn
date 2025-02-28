
#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture[2];

texture2D g_MaskTexture;
texture2D g_BrushTexture;
vector g_vBrushPos = vector(20.f, 0.f, 10.f, 1.f);
float g_fBrushRange = 5.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

// ���ϰ� VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)
/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
/* SV_ : Shader Value : ���� ���̴����� ó���ؾ��� ��� ������ �������� �����δ� �ϰ� �˾Ƽ� �ؾ��� ������ �����ض�. */

/* VertexShader */
/* 1. ������ �⺻���� ��ȯ(����, ��, ����)�� �����Ѵ�.: ���ý����̽����� ���������� ������ı��� ���ؼ� ó�����ִ� ������ �ʿ��ϴ� . */
/* 2. ������ ������ �����Ѵ�.  */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

/* Triangle List�� �׸��� �ִ� ���, ���� ������ ���� ���̴��� ����� ������ ������ ������ ��ġ��. */
/* w������ ������ ���ؼ� ������ �ش��ϴ� ������ ��ġ��. �������� ���������̽�(-1, 1 ~ 1, -1) �� �����ϳ�. */
/* ����Ʈ �����̽��� ��ȯ�Ѵ�. �������� ������ ��ǥ����(0, 0 ~ winsizex, winsizey)�� �����Ѵ�.  */
/* ��������� �����ϰԵȴ�. : ���� ������ �����Ͽ� �ȼ��� ������ �����Ѵ�. */
/* ������ �ȼ��� �ȼ� ���̴��� ������ �ȼ��� ���� �����Ѵ�. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPickedDepth : SV_TARGET3;
};

/* �ȼ��� ���� �����ϳ�. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    vector vSourDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vDestDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);

    vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBrush = (vector) 0.f;

    if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
    {
        float2 vTexcoord = 0.f;

        vTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vTexcoord.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);


        vBrush = g_BrushTexture.Sample(LinearSampler, vTexcoord);
    }

    vector vMtrlDiffuse = vDestDiffuse * vMask + vSourDiffuse * (1.f - vMask) + vBrush;

    Out.vDiffuse = vector(vMtrlDiffuse.rgb, 1.f);

	/* -1 ~ 1 => 0 ~ 1 */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);
    Out.vPickedDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

    return Out;
}

technique11 DefaultTechnique
{
	/* Pass : ���̴� ����� ĸ��ȭ (��������� ��Ƴ���) */

	/* ������ + ������Ʈ + �׸��� + ��Ż�� */
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

