
#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3 g_CamPos;
float g_DeltaTime;
texture2D g_DiffuseTexture, g_NoiseTexture;
bool g_bDead;


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vWorldPos : TEXCOORD2;
};

// 리턴값 VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)
/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
/* SV_ : Shader Value : 내가 셰이더에서 처리해야할 모든 연산을 끝냈으니 앞으로는 니가 알아서 해야할 과정을 수행해라. */

/* VertexShader */
/* 1. 정점의 기본적인 변환(월드, 뷰, 투영)을 수행한다.: 로컬스페이스상의 정점정보를 투영행렬까지 곱해서 처리해주는 과정이 필요하다 . */
/* 2. 정점의 구성을 변경한다.  */
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

/* Triangle List로 그리고 있는 경우, 정점 세개가 정점 쉐이더를 통과할 때까지 모으는 과정을 거치낟. */
/* w나누기 연산을 통해서 투영에 해당하는 과정을 거치낟. 정점들은 투영스페이스(-1, 1 ~ 1, -1) 상에 존재하낟. */
/* 뷰포트 스페이스로 변환한다. 정점들은 윈도우 좌표공간(0, 0 ~ winsizex, winsizey)에 존재한다.  */
/* 래스라이즈를 수행하게된다. : 정점 정보를 보간하여 픽셀의 정보를 생성한다. */
/* 생성된 픽셀을 픽셀 쉐이더로 던져서 픽셀의 색을 결정한다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vWorldPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPickedDepth : SV_TARGET3;
};

/* 픽셀의 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float3 vCamPos = normalize(g_CamPos - In.vWorldPos.xyz);
    float fRim = saturate(dot(In.vNormal.xyz, normalize(vCamPos)));

	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector NoiseTexture = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord);

	/* 알파테스트 더 이상 렌더스테이츠로 설정한다(x) */
    if (vMtrlDiffuse.a <= 0.1f)
        discard;
    
    if (fRim > 0.05f)
        fRim = -9;
    
    fRim = 1 - fRim;
        
    vMtrlDiffuse.xyz = vMtrlDiffuse.xyz - ((1.0 - fRim * 0.1) * 0.3); // 검은색 림
    
    if (g_bDead)
    {
        vMtrlDiffuse.a = vMtrlDiffuse.x;
        vMtrlDiffuse.a -= g_DeltaTime * 0.2f;
    }

    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);
    Out.vPickedDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

technique11 DefaultTechnique
{
	/* Pass : 셰이더 기능의 캡슐화 (여러기법을 모아놓은) */

	/* 빛연산 + 림라이트 + 그림자 + 메탈릭 */
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}

