#include "Boss_Effect_Crack.h"

#include "GameInstance.h"
#include "Player.h"

CBoss_Effect_Crack::CBoss_Effect_Crack(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CBoss_Effect_Crack::CBoss_Effect_Crack(const CBoss_Effect_Crack & rhs)
	: CGameObject( rhs )
{

}

HRESULT CBoss_Effect_Crack::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Effect_Crack::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC*		TransformDesc = static_cast<CTransform::TRANSFORM_DESC*>(pArg);
	TransformDesc->fSpeedPerSec = 1.f;

	if (FAILED(__super::Initialize(TransformDesc)))
		return E_FAIL;
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMat(TransformDesc->worldMat);
	m_pTransformCom->Set_Scale({ 5.5f, 5.5f, 5.5f});
	m_pTransformCom->Go_InitXYZ(3.f, 0.f, 2.5f);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { XMVectorGetX(vPos), -.55f, XMVectorGetZ(vPos), 1.f });
	m_pTransformCom->Rotation({ 1.f, 0.f, 0.f, 1.f }, XMConvertToRadians(90));

	m_fAttack = BOSS_PUNCH_DAMAGE;

	return S_OK;
}

void CBoss_Effect_Crack::PriorityTick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;

	if (m_fDelayTime >= 2.f)
	{
		m_bIsDead = true;
	}
}

void CBoss_Effect_Crack::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CheckColl();
}

void CBoss_Effect_Crack::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CBoss_Effect_Crack::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Effect_Crack::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTexAlpha"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Effect_Earth"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingDesc{};

	BoundingDesc.vExtents = _float3(1.7f, .4f, 1.7f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Effect_Crack::Bind_ShaderResources()
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

void CBoss_Effect_Crack::CheckColl()
{
	if (m_bDamaged)
		return;

	CPlayer* pTargetObject = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
	if (nullptr == pTargetObject)
		return;

	CCollider* pTargetCollider = pTargetObject->GetPlayerColl();
	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		pTargetObject->DoKnockBack(true);
		pTargetObject->SetDamaged(m_fAttack);
		m_bDamaged = true;
	}
}

CBoss_Effect_Crack * CBoss_Effect_Crack::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_Effect_Crack*		pInstance = new CBoss_Effect_Crack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Effect_Crack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBoss_Effect_Crack::Clone(void * pArg)
{
	CBoss_Effect_Crack*		pInstance = new CBoss_Effect_Crack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Effect_Crack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Effect_Crack::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

}
