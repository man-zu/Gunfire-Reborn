#include "Boss_Skill_Shot.h"

#include "GameInstance.h"
#include "LoadModel.h"

#include "Player.h"
#include "Boss.h"

CBoss_Skill_Shot::CBoss_Skill_Shot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CBoss_Skill_Shot::CBoss_Skill_Shot(const CBoss_Skill_Shot & rhs)
	: CGameObject( rhs )
{
}

HRESULT CBoss_Skill_Shot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Skill_Shot::Initialize(void * pArg)
{
	CBoss::BOSSSKILL* ThrowStoneDesc = static_cast<CBoss::BOSSSKILL*>(pArg);

	if (FAILED(__super::Initialize(ThrowStoneDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMat(ThrowStoneDesc->worldMat);
	m_pTransformCom->Go_Right(5.f/ ThrowStoneDesc->fSpeedPerSec);
	m_pTransformCom->Go_Up(7.f / ThrowStoneDesc->fSpeedPerSec);

	m_fAttack = ThrowStoneDesc->fAttack;
	m_pTransformCom->Set_Scale({ 2.f, 2.f , 2.f });

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());
	m_TargetDir = XMVector3Normalize(m_pPlayer->GetPlayerPos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	PLAYONLY(L"BossShot", false);

	return S_OK;
}

void CBoss_Skill_Shot::PriorityTick(_float fTimeDelta)
{
	if (m_fDelayTime >= 13.5f)
		m_bIsDead = true;
}

void CBoss_Skill_Shot::Tick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;

	m_pTransformCom->Go_TargetDir(fTimeDelta, m_TargetDir);

	m_fFrame += 15.f * fTimeDelta;

	if (m_fFrame >= 15.f)
		m_fFrame = 0.f;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CheckColl();
}

void CBoss_Skill_Shot::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	m_pTransformCom->Set_Billboard();
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CBoss_Skill_Shot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (_uint)m_fFrame)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBoss_Skill_Shot::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossBullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingDesc{};

	BoundingDesc.vExtents = _float3(1.f, 1.f, 1.f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Skill_Shot::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CBoss_Skill_Shot::CheckColl()
{
	CPlayer* pTargetObject = m_pPlayer;
	if (nullptr == pTargetObject)
		return;

	CCollider* pTargetCollider = pTargetObject->GetColl();
	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		pTargetObject->SetDamaged(m_fAttack);
		m_bIsDead = true;
	}
}

CBoss_Skill_Shot * CBoss_Skill_Shot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_Skill_Shot*		pInstance = new CBoss_Skill_Shot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Skill_Shot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBoss_Skill_Shot::Clone(void * pArg)
{
	CBoss_Skill_Shot*		pInstance = new CBoss_Skill_Shot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Skill_Shot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Skill_Shot::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
