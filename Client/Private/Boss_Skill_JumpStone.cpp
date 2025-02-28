#include "Boss_Skill_JumpStone.h"

#include "GameInstance.h"
#include "LoadModel.h"
#include "Effect_Common.h"
#include "Player.h"

CBoss_Skill_JumpStone::CBoss_Skill_JumpStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CObject{ pDevice, pContext }
{
}

CBoss_Skill_JumpStone::CBoss_Skill_JumpStone(const CBoss_Skill_JumpStone& rhs)
	: CObject(rhs)
{
}

HRESULT CBoss_Skill_JumpStone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Skill_JumpStone::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CEffect_Common::EFFECT_DESC* pParam = static_cast<CEffect_Common::EFFECT_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { pParam->vInitPos.x, -1.f , pParam->vInitPos.z , 1.f });
	m_pTransformCom->Rotation({ 1.f, 0.f, 0.f }, XMConvertToRadians(90));
	m_pTransformCom->Set_Scale({ 1.f, 1.f, 1.f });
	m_fAttack = BOSS_JUMPSTONE_DAMAGE;

	return S_OK;
}

void CBoss_Skill_JumpStone::PriorityTick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;

	if (m_fDelayTime >= 1.5f)
		m_bIsDead = true;
}

void CBoss_Skill_JumpStone::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	CheckColl();
}

void CBoss_Skill_JumpStone::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBoss_Skill_JumpStone::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < 1; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBoss_Skill_JumpStone::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */ /* CModel Έν */
	if (FAILED(__super::Add_Component(LEVEL_STAGE1_BOSS, L"Prototype_Component_Obj_Boss_JumpStone",
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingAABBDesc{};
	
	BoundingAABBDesc.vExtents = _float3(12.f, 1.f, 12.f);
	BoundingAABBDesc.vCenter = _float3(-1.f, BoundingAABBDesc.vExtents.y, 1.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingAABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Skill_JumpStone::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float3 camPos = (_float3)m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW)->m[3];

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamPos", &camPos, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bDead", &m_bDissolve, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DeltaTime", &m_fDissolveTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CBoss_Skill_JumpStone::CheckColl()
{
	if (m_bColl)
		return;

	CPlayer* pTargetObject = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());
	if (nullptr == pTargetObject)
		return;

	CCollider* pTargetCollider = pTargetObject->GetColl();
	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		pTargetObject->SetDamaged(m_fAttack);
		m_bColl = true;
	}
}

CBoss_Skill_JumpStone* CBoss_Skill_JumpStone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Skill_JumpStone* pInstance = new CBoss_Skill_JumpStone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Skill_JumpStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBoss_Skill_JumpStone::Clone(void* pArg)
{
	CBoss_Skill_JumpStone* pInstance = new CBoss_Skill_JumpStone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Skill_JumpStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Skill_JumpStone::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
