
#include "Engine_Shader_Defines.hlsli"

float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4		g_ViewMatrixInv, g_ProjMatrixInv;
float4x4		g_LightViewMatrix, g_LightProjMatrix;

vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

texture2D		g_NormalTexture;

/* �������� �ʿ��� ��� �ȼ����� ��ǻ�� ������ �ȼ����� �ٸ��� ���ڴ�. */
texture2D		g_DiffuseTexture;

/* �������� �ʿ��� ��� �ȼ����� �ں��Ʈ ������ ��δ� �� ������ �� ĥ�� */
//texture2D		g_AmbientTexture;
vector			g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D		g_ShadeTexture;
texture2D		g_DepthTexture;
texture2D		g_SpecularTexture;
texture2D		g_Texture;
texture2D		g_LightDepthTexture;
vector			g_vCamPosition;

vector			g_vFogColor = vector(1.f, 0.f, 0.f, 1.f);
float			g_fFogRange = 10.0f;

struct VS_IN
{
	float3		vPosition : POSITION;	
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
	float2		vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	

	matrix			matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;		
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

/* �ȼ��� ���� �����ϳ�. */
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;	

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;
	float4		vSpecular : SV_TARGET1;
};

/* ���̵� Ÿ�ٿ� �簢�� ���۸� �׸��� = ������ ���� ��� �������� ����ش�. 48������ �ȼ��� �� �����Ӵ� ���� ������ŭ ������. */
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	vector		vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);	
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
	
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));

	/* �������� �ʿ��� �ȼ��� ������ġ�� �ʿ��ϴ�. */
	/* ������ġ�� ������ ���! */
	/* 1. �������� �ʿ��� ��ü���� �׸� ��, �ϳ��� ����Ÿ���� �� �߰��Ͽ� �ȼ����� ������ġ�� ����޴´�. ������ġ�� �����ϱ⶧���� ����Ÿ���ȼ��� �����ϱ⿡ �������� �ʴ�. */
	/* 2. ������ġ���� �ȼ��� ������ġ�� �߷��غ���. */

	vector		vWorldPos;

	/* ������ġ * ������� * ����� * ������� * 1/w == ���������̽� ���� ��ġ�� ���ϳ�.*/
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* ������ġ * ������� * ����� * ������� */
	vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

	/* ������ġ * ������� * ����� */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ������ġ * ������� */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLook = vWorldPos - g_vCamPosition;

    Out.vSpecular = 0.f; // (g_vLightSpecular * g_vMtrlSpecular); * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	return Out;	
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	vector		vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	/* �������� �ʿ��� �ȼ��� ������ġ�� �ʿ��ϴ�. */
	/* ������ġ�� ������ ���! */
	/* 1. �������� �ʿ��� ��ü���� �׸� ��, �ϳ��� ����Ÿ���� �� �߰��Ͽ� �ȼ����� ������ġ�� ����޴´�. ������ġ�� �����ϱ⶧���� ����Ÿ���ȼ��� �����ϱ⿡ �������� �ʴ�. */
	/* 2. ������ġ���� �ȼ��� ������ġ�� �߷��غ���. */

	vector		vWorldPos;

	/* ������ġ * ������� * ����� * ������� * 1/w == ���������̽� ���� ��ġ�� ���ϳ�.*/
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* ������ġ * ������� * ����� * ������� */
	vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

	/* ������ġ * ������� * ����� */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ������ġ * ������� */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLightDir = vWorldPos - g_vLightPos;

	float		fDistance = length(vLightDir);

	/* �ȼ��� ������ġ�� ������ ��ġ�� ������ 1�� ������. ������ �������� �־����� 0����. */
	float		fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

	Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

	vector		vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	
	vector		vLook = vWorldPos - g_vCamPosition;

    Out.vSpecular = 0.f;	// (g_vLightSpecular * g_vMtrlSpecular); * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f) * fAtt;

	return Out;
}


/* ���������� ����ۿ� �������� ����ֱ����� ���̴�. 48������ �ȼ��� �� �����Ӵ� �ѹ� ������. */
PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (0.0f == vDiffuse.a)
		discard;

	vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);

	vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	Out.vColor = vDiffuse * vShade + vSpecular;


	/* �׸��ڿ����� �Ѵ�.*/
	/* ���� �׷����� �ȼ��� ������������ �÷��̾�� �������ִ� �ȼ��̾��ĸ� �Ǵ�����. */
	/* ���� ȭ�鿡 �׷����� �ȼ��� ������������ ���� ���̸� ������ �ִ����� ������.*/	
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
	float		fViewZ = vDepthDesc.y * 3000.f;

	vector		vWorldPos;

	/* ������ġ * ������� * ����� * ������� * 1/w == ���������̽� ���� ��ġ�� ���ϳ�.*/
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* ������ġ * ������� * ����� * ������� */
	vWorldPos = vWorldPos * fViewZ;

	/* ������ġ * ������� * ����� */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ������ġ * ������� */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vWorldPos = mul(vWorldPos, g_LightViewMatrix);
	vWorldPos = mul(vWorldPos, g_LightProjMatrix);

	float		fLightDepth = vWorldPos.w;

	vWorldPos /= vWorldPos.w;

	float2		vLightDepthTexcoord;

	vLightDepthTexcoord.x = vWorldPos.x * 0.5f + 0.5f;	
	vLightDepthTexcoord.y = vWorldPos.y * -0.5f + 0.5f;	

	vector		vOldLightDepth = g_LightDepthTexture.Sample(LinearSampler, vLightDepthTexcoord);
	float		fOldLightViewDepth = vOldLightDepth.y * 3000.f;

	if (fLightDepth - 0.1f > fOldLightViewDepth)
		Out.vColor *= 0.5f;

	return Out;
}

PS_OUT PS_MAIN_DEFERRED_FOG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (0.0f == vDiffuse.a)
		discard;

	vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);

	vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);

	float		fViewZ = vDepthDesc.y * 3000.f;

	float		fFogPower = max((fViewZ - g_fFogRange) / 10.f, 0.f);
	/*fViewZ
	
	g_fFogRange;*/

	Out.vColor = vDiffuse * vShade + vSpecular;

	// Out.vColor = Out.vColor + g_vFogColor * fFogPower;

	Out.vColor = Out.vColor * (1.f - fFogPower) + g_vFogColor * fFogPower;	

	return Out;
}


technique11			DefaultTechnique
{
	pass RenderTarget_Debug
	{		
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Sky, 0);
		SetBlendState(BS_LightBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Sky, 0);
		SetBlendState(BS_LightBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Deferred
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Sky, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
	}

	pass Deferred_Fog
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Sky, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_FOG();
	}
}

