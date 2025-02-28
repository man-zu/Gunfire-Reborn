#include "UI_PlayerShieldBar.h"

#include "GameInstance.h"
#include "Player.h"

CUI_PlayerShieldBar::CUI_PlayerShieldBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_PlayerShieldBar::CUI_PlayerShieldBar(const CUI_PlayerShieldBar & rhs)
	: CUI( rhs )
{
}

HRESULT CUI_PlayerShieldBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerShieldBar::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_tUIDesc.fSizeX = 150.f;
	m_tUIDesc.fSizeY = 8.f;
	m_tUIDesc.fX = -(g_iWinSizeX * 0.5f) + 258.f - m_tUIDesc.fSizeX * 0.5f;
	m_tUIDesc.fY = (g_iWinSizeY * 0.5f) - 639.f;

	m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, 0.f, 1.f));

	//m_pTransformCom->Rotation({ 0.f, 0.f, 1.f }, XMConvertToRadians(2));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));



	return S_OK;
}

void CUI_PlayerShieldBar::PriorityTick(_float fTimeDelta)
{

	if (!m_pPlayer)
		m_pPlayer = dynamic_cast<CPlayer*>(GET_OBJ(LEVEL_STATIC, L"Layer_Player", 0));

	m_fQuantity = _float(m_pPlayer->GetShield()) / _float(m_pPlayer->GetMaxShield());
}

void CUI_PlayerShieldBar::Tick(_float fTimeDelta)
{
}

void CUI_PlayerShieldBar::LateTick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX * m_fQuantity + 0.1f, m_tUIDesc.fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet((m_tUIDesc.fX + (m_tUIDesc.fSizeX * m_fQuantity * 0.5f)), m_tUIDesc.fY, 0.f, 1.f));

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_PlayerShieldBar::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerShieldBar::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Character_SFBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_PlayerShieldBar * CUI_PlayerShieldBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_PlayerShieldBar*		pInstance = new CUI_PlayerShieldBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_PlayerShieldBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_PlayerShieldBar::Clone(void * pArg)
{
	CUI_PlayerShieldBar*		pInstance = new CUI_PlayerShieldBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_PlayerShieldBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerShieldBar::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
