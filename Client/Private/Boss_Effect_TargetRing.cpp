#include "Boss_Effect_TargetRing.h"

#include "GameInstance.h"
#include "Level.h"

CBoss_Effect_TargetRing::CBoss_Effect_TargetRing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CBoss_Effect_TargetRing::CBoss_Effect_TargetRing(const CBoss_Effect_TargetRing & rhs)
	: CGameObject( rhs )
{

}

HRESULT CBoss_Effect_TargetRing::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Effect_TargetRing::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;


	EFFECT_DESC* effectDesc = static_cast<EFFECT_DESC*>(pArg);
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&effectDesc->vInitPos));
	m_pTransformCom->Rotation({ 1.f, 0.f, 0.f, 1.f }, XMConvertToRadians(90));

	return S_OK;
}

void CBoss_Effect_TargetRing::PriorityTick(_float fTimeDelta)
{
}

void CBoss_Effect_TargetRing::Tick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta * 20.f;

	if (m_fDelayTime >= 34.f)
	{
		m_bIsDead = true;
	}

	m_pTransformCom->Set_Scale({ m_fDelayTime, m_fDelayTime, m_fDelayTime });

}

void CBoss_Effect_TargetRing::LateTick(_float fTimeDelta)
{

	if(!m_bIsDead)
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CBoss_Effect_TargetRing::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Effect_TargetRing::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Effect_TargetRing"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Effect_TargetRing::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 16)))
		return E_FAIL;

	return S_OK;
}

CBoss_Effect_TargetRing * CBoss_Effect_TargetRing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_Effect_TargetRing*		pInstance = new CBoss_Effect_TargetRing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Effect_TargetRing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBoss_Effect_TargetRing::Clone(void * pArg)
{
	CBoss_Effect_TargetRing*		pInstance = new CBoss_Effect_TargetRing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Effect_TargetRing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Effect_TargetRing::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

}
