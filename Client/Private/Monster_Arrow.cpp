#include "Monster_Arrow.h"

#include "GameInstance.h"
#include "Player.h"
#include "Monster_Effect.h"

CMonster_Arrow::CMonster_Arrow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster{ pDevice, pContext }
{
}

CMonster_Arrow::CMonster_Arrow(const CMonster_Arrow & rhs)
	: CMonster(rhs)
{
}

const _float4x4 * CMonster_Arrow::Get_BoneMatrixPtr(const _char * pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);	
}

HRESULT CMonster_Arrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Arrow::Initialize(void * pArg)
{
	m_eMonsterState = MONSTER_IDLE;

	CTransform::TRANSFORM_DESC trans;
	trans.fSpeedPerSec = 3.5f;
	trans.fRotationPerSec = 1.f;

	if (FAILED(__super::Initialize(&trans)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Navi(pArg)))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0, true);

	/* 최초 위치 설정 */
	m_pPosMove = static_cast<CTransform::TRANSFORM_DESC*>(pArg)->vInitPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pPosMove));

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	m_iHp = m_iMaxHp = 100;
	m_fShield = m_fMaxShield = 30.f;
	m_fAttack = 30.f;

	return S_OK;
}

void CMonster_Arrow::PriorityTick(_float fTimeDelta)
{
	if (m_iHp <= 0)
	{
		m_fDelayTime += fTimeDelta;

		m_bDissolve = true;

		if (m_fDelayTime >= 2.f)
		{
			Set_Dead(true);
			m_pPlayer->AddKillCnt();
		}

		if (!m_bDieEffect)
		{
			m_bDieEffect = true;
			PLAYONLY(L"ArrowDie", false);

			CMonster_Effect::MON_EFFECT_DESC effectDesc{};
			XMStoreFloat4(&effectDesc.vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			effectDesc.pNavi = m_pNavigationCom;
			FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Effect", L"Prototype_Component_GameObject_Monster_Effect", &effectDesc));
		}
	}
}

void CMonster_Arrow::Tick(_float fTimeDelta)
{
	/*
	+	[0]	"(2161_Stand)--2161_Stand"
	+	[1]	"(2161_Stand_Incombat)--2161_Stand_InCombat"
	+	[2]	"(2161_Squat)--2161_Squat"
	+	[3]	"(2161_Run_F)--2161_Run_F"
	+	[4]	"(2161_Patrol)--2161_Patrol"
	+	[5]	"(2161_Fire01)--2161_WeaponKeep"
	+	[6]	 <문자열에 잘못된 문자가 있습니다>
	+	[7]	 <문자열에 잘못된 문자가 있습니다>
	+	[8]	 <문자열에 잘못된 문자가 있습니다>
	+	[9]	"(2161_Jump)--2161_JumpStart"
	+	[10] "(2161_Jump)--2161_Land" 
	+	[11] "(2161_Attack01)--2161_Attack01" 
	+	[12] "(2161_walk_F)--2161_Walk_F" 
	+	[13] "(2161_walk_R)--2161_Walk_R" 
	+	[14] "(2161_walk_B)--2161_Walk_B" 
	+	[15] "(2161_walk_L)--2161_Walk_L" 
	+	[16] "(2161_Fire01)--2161_SquatFire" 
	+	[17] "(2161_Reload)--2161_Reload" 
	
	(너무 멀면)IDLE 0, 1
	걷기 12
	달리기 3
	장전 17
	공격
		- 플레이어 장거리 인식 : 활쏘기 (2)
		- 근거리 인식 : 활로 때리기 (11)
	피격 7
	
	*/
	if (m_eMonsterState == MONSTER_HIT)
		motion_hit();
	if (m_eMonsterState != MONSTER_HIT && m_iHp > 0)
	{
		motion_idle();
		motion_moveToPlayer(fTimeDelta);
		//motion_moveToInitPos(fTimeDelta);
		motion_attack();
	}

	if (KEY_PUSH(DIK_P))
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

	m_pModelCom->Play_Animation(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMonster_Arrow::LateTick(_float fTimeDelta)
{
	_float4 vCurPos;
	XMStoreFloat4(&vCurPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	vCurPos.y = -0.15f + m_pNavigationCom->GetHeight(XMLoadFloat4(&vCurPos), m_pNavigationCom->GetCurrIdx());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vCurPos));

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_SHADOWGAMEOBJECT, this);
}

HRESULT CMonster_Arrow::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NoiseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CMonster_Arrow::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_arrow"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingAABBDesc{};
	
	BoundingAABBDesc.vExtents = _float3(.5f, 1.5f, .5f);
	BoundingAABBDesc.vCenter = _float3(0.f, BoundingAABBDesc.vExtents.y, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingAABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Arrow::Ready_Navi(void* pArg)
{
	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC		NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = static_cast<CMonster::MONSTER_DESC*>(pArg)->iCurrentCellIndex;

	if (static_cast<CMonster::MONSTER_DESC*>(pArg)->eLevel == LEVEL_STAGE1)
	{
		if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Prototype_Component_Navigation_Stage1"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}
	if (static_cast<CMonster::MONSTER_DESC*>(pArg)->eLevel == LEVEL_STAGE2)
	{
		if (FAILED(__super::Add_Component(LEVEL_STAGE2, TEXT("Prototype_Component_Navigation_Stage2"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_Arrow::Bind_ShaderResources()
{
	_float4x4 mat, matInv;
	XMStoreFloat4x4(&mat, XMMatrixIdentity());
	XMStoreFloat4x4(&matInv, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_partMatrix", &mat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_partMatrixInv", &matInv)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float3 camPos = (_float3)m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW)->m[3];
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamPos", &camPos, sizeof(_float3))))
		return E_FAIL;

	if (m_eMonsterState == MONSTER_HIT)
		m_bRim = true;
	else
		m_bRim = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRedRim", &m_bRim, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bDead", &m_bDissolve, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DeltaTime", &m_fDelayTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CMonster_Arrow::motion_moveToPlayer(_float fTimeDelta)
{
	if ((m_eMonsterState == MONSTER_ATTACK)
		|| !m_pTransformCom->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 15.f))
		return;

	m_eMonsterState = MONSTER_MOVE_PLAYER;

	if (0 == m_iStateCurNum)
	{
		m_pTransformCom->LookAt(m_pPlayer->GetPlayerPos());
		m_pModelCom->Set_AnimIndex(3, true);
		m_iStateCurNum = 1;
	}

	// 플레이어랑 가까워지면 달리기 stop
	if (1 == m_iStateCurNum)
	{
		m_pTransformCom->Go_Target(fTimeDelta, m_pPlayer->GetPlayerPos(), m_pNavigationCom);

		if (m_pTransformCom->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 10.f))
		{
			m_iStateCurNum = 2;
		}
	}

	if (2 == m_iStateCurNum)
	{
		m_eMonsterState = MONSTER_ATTACK;
		m_pModelCom->Set_AnimIndex(2, false);
		m_iStateCurNum = 0;
	}
}

void CMonster_Arrow::motion_moveToInitPos(_float fTimeDelta)
{
	if (m_eMonsterState != MONSTER_MOVE_INIT)
		return;

	if (m_pTransformCom->Go_DistanceCheck(XMLoadFloat4(&m_pPosMove), 1.5f))
	{
		m_eMonsterState = MONSTER_IDLE;
		m_iStateCurNum = 1;
		return;
	}

	if (0 == m_iStateCurNum)
	{
		m_pTransformCom->LookAt(XMLoadFloat4(&m_pPosMove));
		m_pModelCom->Set_AnimIndex(3, true);
		m_iStateCurNum = 1;
	}

	// 플레이어랑 가까워지면 달리기 stop
	if (1 == m_iStateCurNum)
	{
		m_pTransformCom->Go_Target(fTimeDelta, XMLoadFloat4(&m_pPosMove));

		if (m_pTransformCom->Go_DistanceCheck(XMLoadFloat4(&m_pPosMove), 7.f))
		{
			m_iStateCurNum = 2;
		}
	}

	if (2 == m_iStateCurNum)
	{
		m_pModelCom->Set_AnimStop();
		motion_reset();
	}
}

void CMonster_Arrow::motion_attack()
{
	if (m_eMonsterState != MONSTER_ATTACK)
		return;

	//if (!m_pTransformCom->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 15.5f))
	//{
	//	m_eMonsterState = MONSTER_MOVE_INIT;
	//	m_iStateCurNum = 0;
	//}
	m_pTransformCom->LookAt(m_pPlayer->GetPlayerPos());

	if (m_iStateCurNum == 0)
	{
		if(m_iShotCnt <= 3 && m_pModelCom->Get_Finished())
		{
			m_pModelCom->Set_AnimIndex(2, false);	// 활쏘기
			m_iShotCnt++;

			createArrow();
		}

		if(m_iShotCnt >= 4)
		{
			m_pModelCom->Set_AnimIndex(17, false);	// 장전
			m_iShotCnt = 0;
		}
	}
}

void CMonster_Arrow::motion_hit()
{
	if (m_eMonsterState != MONSTER_HIT)
		return;

	if (m_iStateCurNum == 0)
	{
		m_pModelCom->Set_AnimIndex(6, false);
		m_iStateCurNum = 1;
	}

	if (1 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		motion_reset();
	}
}

void CMonster_Arrow::motion_idle()
{
	if (m_eMonsterState != MONSTER_IDLE)
		return;

	if (m_iStateCurNum == 1)
	{
		m_pModelCom->Set_AnimIndex(1, true);
		m_iStateCurNum = 0;
	}

}

void CMonster_Arrow::motion_reset()
{
	m_iStateCurNum = 0;
	m_eMonsterState = MONSTER_IDLE;
}

void CMonster_Arrow::createArrow()
{
	_vector monPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	monPos.m128_f32[1] += 0.8f;

	CTransform::TRANSFORM_DESC desc{};
	XMStoreFloat4(&desc.vInitPos, monPos);
	desc.fSpeedPerSec = 10.f;

	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Arrow_Bullet", L"Prototype_GameObject_Monster_Arrow_bullet", &desc);

}

void CMonster_Arrow::CheckColl()
{
	//CPlayer* pTargetObject = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
	//if (nullptr == pTargetObject)
	//	return;
	//
	//CCollider* pTargetCollider = pTargetObject->GetPlayerColl();
	//if (nullptr == pTargetCollider)
	//	return;
	//
	//if (m_pColliderCom->Intersect(pTargetCollider))
	//{
	//	pTargetObject->SetDamaged(m_fAttack);
	//
	//	m_eMonsterState = DIE;
	//}
}

CMonster_Arrow * CMonster_Arrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster_Arrow*		pInstance = new CMonster_Arrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Arrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonster_Arrow::Clone(void * pArg)
{
	CMonster_Arrow*		pInstance = new CMonster_Arrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Arrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMonster_Arrow::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
}
