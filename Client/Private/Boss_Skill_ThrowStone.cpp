#include "Boss_Skill_ThrowStone.h"

#include "GameInstance.h"
#include "LoadModel.h"

#include "Player.h"
#include "Effect_Common.h"

CBoss_Skill_ThrowStone::CBoss_Skill_ThrowStone(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObject { pDevice, pContext }
{
}

CBoss_Skill_ThrowStone::CBoss_Skill_ThrowStone(const CBoss_Skill_ThrowStone & rhs)
	: CObject( rhs )
{
}

HRESULT CBoss_Skill_ThrowStone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Skill_ThrowStone::Initialize(void * pArg)
{
	THROWSTONE*		ThrowStoneDesc = static_cast<THROWSTONE*>(pArg);

	if (FAILED(__super::Initialize(ThrowStoneDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMat(ThrowStoneDesc->worldMat);
	m_pTransformCom->Go_InitXYZ(5.5f / ThrowStoneDesc->fSpeedPerSec, 7.f / ThrowStoneDesc->fSpeedPerSec, 0.f);

	m_OrginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_OrginScale = m_pTransformCom->Get_Scaled();
	m_pTransformCom->Set_Scale({ m_OrginScale.x * m_fSize, m_OrginScale.y * m_fSize , m_OrginScale.z * m_fSize });

	m_fAttack = BOSS_THROWSTONE_DAMAGE;

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	return S_OK;
}

void CBoss_Skill_ThrowStone::PriorityTick(_float fTimeDelta)
{
	if (m_fDelayTime >= 5.f)
	{
		m_bIsDead = true;

		_vector curPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		/* È¿°ú */
		CEffect_Common::EFFECT_DESC effectDesc{};
		effectDesc.vInitPos = { XMVectorGetX(curPos), XMVectorGetY(curPos), XMVectorGetZ(curPos), 1.f};
		effectDesc.iImgCnt = 17;
		effectDesc.vScale = { 3.f, 3.f, 3.f };
		effectDesc.sTextureTag = L"Prototype_Component_Texture_Effect_gray";
		
		FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Effect", L"Prototype_Component_Effect_Common", &effectDesc));
	}

	m_fDelayTime += fTimeDelta;

	if (m_fDelayTime <= 2.f)
	{
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, fTimeDelta);

		if (m_fDelayTime < .4f)
			m_fSize += 0.001f;

		if (m_fDelayTime <= 1.5f)
			m_fSize += 0.00001f;

		m_pTransformCom->Set_Scale({ m_OrginScale.x * m_fSize, m_OrginScale.y * m_fSize , m_OrginScale.z * m_fSize });
	}

	if (m_fDelayTime < 1.7f && m_fDelayTime >= 1.5f)
		m_TargetDir = XMVector3Normalize(m_pPlayer->GetPlayerPos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_fDelayTime >= 2.f)
		int a = 0;

	if (m_fDelayTime >= 1.7f)
		m_pTransformCom->Go_TargetDir(fTimeDelta, m_TargetDir);

}

void CBoss_Skill_ThrowStone::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CheckColl();
}

void CBoss_Skill_ThrowStone::LateTick(_float fTimeDelta)
{

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBoss_Skill_ThrowStone::Render()
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

HRESULT CBoss_Skill_ThrowStone::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */ /* CModel ¸í */
	if (FAILED(__super::Add_Component(LEVEL_STAGE1_BOSS, L"Prototype_Component_Obj_stone_1",
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingDesc{};

	BoundingDesc.vExtents = _float3(10.f, 10.f, 10.f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Skill_ThrowStone::Bind_ShaderResources()
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

	return S_OK;
}

void CBoss_Skill_ThrowStone::CheckColl()
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

CBoss_Skill_ThrowStone * CBoss_Skill_ThrowStone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_Skill_ThrowStone*		pInstance = new CBoss_Skill_ThrowStone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Skill_ThrowStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBoss_Skill_ThrowStone::Clone(void * pArg)
{
	CBoss_Skill_ThrowStone*		pInstance = new CBoss_Skill_ThrowStone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Skill_ThrowStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Skill_ThrowStone::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
