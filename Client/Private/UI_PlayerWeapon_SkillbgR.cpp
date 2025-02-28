#include "UI_PlayerWeapon_SkillbgR.h"

#include "GameInstance.h"

CUI_PlayerWeapon_SkillbgR::CUI_PlayerWeapon_SkillbgR(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_PlayerWeapon_SkillbgR::CUI_PlayerWeapon_SkillbgR(const CUI_PlayerWeapon_SkillbgR & rhs)
	: CUI( rhs )
{
}

HRESULT CUI_PlayerWeapon_SkillbgR::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerWeapon_SkillbgR::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	/* Binding pArg */
	{
		if (pArg == nullptr)
			return E_FAIL;

		m_tUIDesc.fX = static_cast<CUI::UIDESC*>(pArg)->fX - 0.7f;
		m_tUIDesc.fY = static_cast<CUI::UIDESC*>(pArg)->fY + 0.4f;
		m_iTextureIdx = static_cast<CUI::UIDESC*>(pArg)->iTextureIdx;
	}

	m_tUIDesc.fSizeX = 64.f * 0.5f;
	m_tUIDesc.fSizeY = 64.f * 0.5f;

	m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));



	return S_OK;
}

void CUI_PlayerWeapon_SkillbgR::PriorityTick(_float fTimeDelta)
{
}

void CUI_PlayerWeapon_SkillbgR::Tick(_float fTimeDelta)
{
}

void CUI_PlayerWeapon_SkillbgR::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_PlayerWeapon_SkillbgR::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerWeapon_SkillbgR::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Skill"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_PlayerWeapon_SkillbgR * CUI_PlayerWeapon_SkillbgR::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_PlayerWeapon_SkillbgR*		pInstance = new CUI_PlayerWeapon_SkillbgR(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_PlayerWeapon_SkillbgR"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_PlayerWeapon_SkillbgR::Clone(void * pArg)
{
	CUI_PlayerWeapon_SkillbgR*		pInstance = new CUI_PlayerWeapon_SkillbgR(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_PlayerWeapon_SkillbgR"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerWeapon_SkillbgR::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
