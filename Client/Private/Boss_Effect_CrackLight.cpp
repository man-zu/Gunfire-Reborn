#include "Boss_Effect_CrackLight.h"

#include "GameInstance.h"
#include "Player.h"

CBoss_Effect_CrackLight::CBoss_Effect_CrackLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CBoss_Effect_CrackLight::CBoss_Effect_CrackLight(const CBoss_Effect_CrackLight & rhs)
	: CGameObject( rhs )
{

}

HRESULT CBoss_Effect_CrackLight::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Effect_CrackLight::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC*		TransformDesc = static_cast<CTransform::TRANSFORM_DESC*>(pArg);
	TransformDesc->fSpeedPerSec = 1.f;

	if (FAILED(__super::Initialize(TransformDesc)))
		return E_FAIL;
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMat(TransformDesc->worldMat);
	m_pTransformCom->Go_InitXYZ(3.f, 0.f, 2.5f);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { XMVectorGetX(vPos), -.6f, XMVectorGetZ(vPos), 1.f });
	m_pTransformCom->Rotation({ 1.f, 0.f, 0.f, 1.f }, XMConvertToRadians(90));

	return S_OK;
}

void CBoss_Effect_CrackLight::PriorityTick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;

	if (m_fDelayTime >= 3.f)
	{
		m_bIsDead = true;
	}


	if (m_fAlphaChange <= 1.f)
	{
		m_fAlphaChange += fTimeDelta;
		m_pTransformCom->Set_Scale({ m_fAlphaChange * 90.f, m_fAlphaChange * 90.f, m_fAlphaChange * 90.f });
	}

}

void CBoss_Effect_CrackLight::Tick(_float fTimeDelta)
{
	CheckColl();
}

void CBoss_Effect_CrackLight::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CBoss_Effect_CrackLight::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Effect_CrackLight::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTexAlpha"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Effect_GlowCp"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Effect_CrackLight::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_AlphaChange", &m_fAlphaChange, sizeof(_float))))
		return E_FAIL;

	_bool bAlphaAdd = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bAlphaAdd", &bAlphaAdd, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

void CBoss_Effect_CrackLight::CheckColl()
{
}

CBoss_Effect_CrackLight * CBoss_Effect_CrackLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_Effect_CrackLight*		pInstance = new CBoss_Effect_CrackLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Effect_CrackLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBoss_Effect_CrackLight::Clone(void * pArg)
{
	CBoss_Effect_CrackLight*		pInstance = new CBoss_Effect_CrackLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Effect_CrackLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Effect_CrackLight::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

}
