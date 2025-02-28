#include "..\Public\Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"

#include "VIBuffer_Rect.h"
#include "Shader.h"

_uint		g_iSizeX = 8192;
_uint		g_iSizeY = 4608;


CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }

{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	/* 디퍼드 쉐이더를 위한 렌더타겟들을 추가하겠다. */

	/* 원래는 백 버퍼에 그려졌어야할 내용 그대로 다른 렌더타겟에 그리는 작업을 하므로 */
	/* 생성하는 렌더타겟들의 크기를 백버퍼와 맞추겠다. */
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* For.Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_PickedDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PickedDepth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_LightDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* For.MRT_GameObjects ( Diffuse + Normal + Depth ) */
	/* 이 그룹에 추가된 렌더타겟들을 객체로부터 받아와야할 정보를 의마한ㄷ.ㅏ */
	/* 객체들을 그리기 직전에 바인딩 되어야할 타겟들이다. */
	/* 카메라기준 원근투영으로 그린다. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_PickedDepth"))))
		return E_FAIL;


	/* For.MRT_LightAcc ( Shade + Specular )*/
	/* 이 그룹에 추가된 렌더타겟들을 빛들로부터 받아와야할 정보를 의마한ㄷ.ㅏ */
	/* 빛연산을 하기 직전에 바인딩 되어야할 타겟들이다. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* For.MRT_ShadowGameObject ( LightDepth )*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowGameObject"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	/* 화면을 꽉 채워서 직교투영해주기위한 월드행렬 */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;

	/*m_WorldMatrix._41 = ViewportDesc.Width * 0.5f - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -ViewportDesc.Height * 0.5f + ViewportDesc.Height * 0.5f;*/

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;


	/* 사이즈를 확장한 깊이버퍼를 만들어놓는다. */

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthDSView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);


#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Diffuse"), 100.f, 200.f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Normal"), 100.f, 250.0f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Depth"), 100.f, 300.0f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Shade"), 100.f, 350.0f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Specular"), 100.f, 400.0f, 50.f, 50.f)))
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderObjects(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	/* 백버퍼에 스카이박스를 그리낟.  */
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_ShadowGameObject()))
		return E_FAIL;


	/* 지형 + 프레이어 + 몬스터 빛 연산이 필요한 객체들이다. */
	/* 다른 타겟에 빛연산에 필요한 데이터들을 렌더링(저장)해주기위해 (빛연산을 할때 픽셀당 디퓨즈 재질 + 픽셀당 노말벡터) */
	/* 백버퍼를 잠시 빼두고 데이터들을 저장받기위한 렌더타겟으로 교체하는 작업을 하겠다. */
	/* 결론적으로는 디퓨즈, 노말타겟에 빛연산이 필요한 객체들의 픽셀(디퓨즈재질, 노말벡터의 정보를 저장했다.) */
	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	/* 빛연산한 결과를 Shade타겟에 저장받는다. */
	/* Shade타겟을 장치에 바인딩작업이 필요하다.*/
	/* 뭔가를 그리낟 == 사각형 버퍼를 직교투영으로 그린다. shade타겟을 꽉채워서 그리는 사가형을 그리겠다. */
	/* 사각형을 그릴때 빛 정보와 디퓨즈와 노말을 이용해서 빛연산한 결과를 Shade타겟에 그려줄꺼다. */
	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	/* Diffuse Shade 두개의 타겟을 곱해서 백버퍼에 그려준다. */
	/* 사각형을 직교투영으로 그린다. 그리위해서는 쉐이더가 필요해., */
	/* 이 쉐이더에 Diffuse SRV, Shade SRV두개를 던져서 곱해서 찍는다. */
	if (FAILED(Render_Deferred()))
		return E_FAIL;


	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG

	if (CInput_Device::PUSH == m_pGameInstance->Get_DIKeyState(DIK_CAPSLOCK))
		m_bShow = !m_bShow;

	if (m_bShow)
	{
		if (FAILED(Render_Debug()))
			return E_FAIL;
	}

#endif

	return S_OK;
}

void CRenderer::Clear()
{
	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ShadowGameObject()
{
	/* 그림자를 그려내기위한 객체들의 광원으로부터의 깊이를 저장하고자한다. */
	/* 저장하기위한 타겟 == Target_LightDepth */
	m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowGameObject"), m_pLightDepthDSView);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = g_iSizeX;
	ViewPortDesc.Height = g_iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOWGAMEOBJECT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_LightDepth();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_SHADOWGAMEOBJECT].clear();

	m_pGameInstance->End_MRT();

	m_pGameInstance->Clear_DepthStencil_View();

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = 1280.0f;
	ViewPortDesc.Height = 720.0f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal + Depth + Hit 타겟을 바인딩한다. */	
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONBLEND].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{

	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* Shade */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	/* 셋팅해놓은 쉐이드 타겟에 모든 픽셀을 채워주기위해서. */
	/* 쉐이드 타겟의 사이즈만큼 직교투영된 사각형 버퍼를 그린다. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	/* 빛 갯수만큼 사각형을 그리낟. */
	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_LightDepth"), m_pShader, "g_LightDepthTexture")))
		return E_FAIL;
	

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	/* 광원기준으로 표현된 뷰, 투영행렬이다. (카메라기준x) */
	_float4x4		LightViewMatrix, LightProjMatrix;

	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(XMVectorSet(-10.f, 30.f, -10.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&LightProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), (_float)1280.0f / 720.0f, 0.1f, 3000.f));

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &LightViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
		return E_FAIL;


	m_pShader->Begin(3);

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	m_pVIBuffer->Render();
	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_ShadowGameObject"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

#ifdef _DEBUG
	for (auto& pDebugCom : m_DebugComponents)
		Safe_Release(pDebugCom);
	m_DebugComponents.clear();
#endif

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}
	Safe_Release(m_pLightDepthDSView);
	
	Safe_Release(m_pDevice); 
	Safe_Release(m_pContext);
}
