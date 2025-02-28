#include "Monster_Sword.h"

#include "GameInstance.h"
#include "Player.h"
#include "Monster_Effect.h"

CMonster_Sword::CMonster_Sword(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster{ pDevice, pContext }
{
}

CMonster_Sword::CMonster_Sword(const CMonster_Sword & rhs)
	: CMonster(rhs)
{
}

const _float4x4 * CMonster_Sword::Get_BoneMatrixPtr(const _char * pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);	
}

HRESULT CMonster_Sword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Sword::Initialize(void * pArg)
{
	m_eMonsterState = MONSTER_IDLE;

	CTransform::TRANSFORM_DESC trans;
	trans.fSpeedPerSec = 2.5f;
	trans.fRotationPerSec = 1.f;

	if (FAILED(__super::Initialize(&trans)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Navi(pArg)))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(3, true);	// 3

	/* 최초 위치 설정 */
	m_pPosMove = static_cast<CTransform::TRANSFORM_DESC*>(pArg)->vInitPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pPosMove));

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	m_iHp = m_iMaxHp = 100;
	m_fShield = m_fMaxShield = 30.f;
	m_fAttack = 30.f;

	return S_OK;
}

void CMonster_Sword::PriorityTick(_float fTimeDelta)
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
			PLAYONLY(L"swordDie", false);

			CMonster_Effect::MON_EFFECT_DESC effectDesc{};
			XMStoreFloat4(&effectDesc.vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			effectDesc.pNavi = m_pNavigationCom;
			FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Effect", L"Prototype_Component_GameObject_Monster_Effect", &effectDesc));
		}
	}

	__super::ColDelay(fTimeDelta);
}

void CMonster_Sword::Tick(_float fTimeDelta)
{
	/*
	+ [0] "(2001_Stand_InCombat)--Take 001"
	+ [1] "(2001_Run)--2001_Run"
	+ [2] "(2001_Run02)--2001_Run02"
	+ [3] "(2001_Stand)--Take 001"
	+ [4] "(2001_Hit_Head)--2001_Hit_Head+"
	+ [5] "(2001_Standup)--2081_Standup"
	+ [6] "(2001_Standup)--2081_Squat"
	+ [7] "(2081_Show1)--2081_Show" 
	+ [8] "(2001_Jump)--2001_Hide_Jump"
	+ [9] "(2001_Jump)--2001_Hide_Land"
	+ [10] "(2001_Hit_body)--2001_Hit_body+"
	+ [11] "(2001_Hide_Right)--Take 001"
	+ [12] "(2001_Hide_Left)--Take 001"
	+ [13] "(2001_Attack04)--2001_Attack02+"
	+ [14] "(2001_Attack03)--2001_Attack01++"
	+ [15] "(2007_Throw01)--2007_Throw01"

	IDLE 0, 3(정자세)
	걷기 없음
	달리기 1, 2
	공격
	 - 13 14
	피격 4 10
	*/
	if (m_pPlayer->GetPlayerState() == CPlayer::DIE
		|| (m_eMonsterState != MONSTER_MOVE_INIT
		&& !m_pTransformCom->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 10.5f)))
	{
		m_eMonsterState = MONSTER_MOVE_INIT;
		m_iStateCurNum = 0;
	}
	if (m_eMonsterState == MONSTER_HIT)
		motion_hit();
	else if (m_eMonsterState != MONSTER_HIT && m_iHp > 0)
	{
		//m_pTransformCom->LookAtForLandObject(m_pPlayer->GetPlayerPos());

		motion_moveToPlayer(fTimeDelta);
		motion_moveToInitPos(fTimeDelta);
		motion_attack();
		motion_idle();
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom_Weapon->Update(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bip001 R Finger0Nub")) * m_pTransformCom->Get_WorldMatrix());

	CheckColl();
}

void CMonster_Sword::LateTick(_float fTimeDelta)
{
	_float4 vCurPos;
	_float fNaviHeight = m_pNavigationCom->GetHeight(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pNavigationCom->GetCurrIdx());
	XMStoreFloat4(&vCurPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	vCurPos.y = 0.f + fNaviHeight;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vCurPos));

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pColliderCom_Weapon);
#endif
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

	if (m_fDelayTime <= 1.f)
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_SHADOWGAMEOBJECT, this);
}

HRESULT CMonster_Sword::Render()
{
//#ifdef _DEBUG
//	string idx = "idx : " + to_string(m_tmp);
//	wstring tmp;
//	tmp.assign(idx.begin(), idx.end());
//	
//	m_pGameInstance->WriteText(TEXT("Font_Default"), tmp, _float2(0.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
//#endif

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

HRESULT CMonster_Sword::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_sword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingAABBDesc{};
	
	BoundingAABBDesc.vExtents = _float3(.5f, 1.f, .5f);
	BoundingAABBDesc.vCenter = _float3(0.f, BoundingAABBDesc.vExtents.y, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingAABBDesc)))
		return E_FAIL;

	// monster_body_2001_15
	CBounding_OBB::BOUNDING_OBB_DESC	BoundingDesc{};
	BoundingDesc.vExtents = _float3(1.f, 1.f, 1.f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRotation = _float3(0.f, 0.0f, -0.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider2"), reinterpret_cast<CComponent**>(&m_pColliderCom_Weapon), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Sword::Ready_Navi(void* pArg)
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

HRESULT CMonster_Sword::Bind_ShaderResources()
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

void CMonster_Sword::motion_moveToPlayer(_float fTimeDelta)
{
	if ((m_eMonsterState == MONSTER_ATTACK || m_eMonsterState == MONSTER_MOVE_INIT)
		|| !m_pTransformCom->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 18.f))
		return;

	if (m_eMonsterState == MONSTER_IDLE && m_iStateCurNum != 0)
		m_iStateCurNum = 0;

	m_eMonsterState = MONSTER_MOVE_PLAYER;

	//	달리기 1, 2

	if (0 == m_iStateCurNum)
	{
		m_pTransformCom->LookAt(m_pPlayer->GetPlayerPos());
		m_pModelCom->Set_AnimIndex(2, true);
		m_iStateCurNum = 1;
	}
	
	// 플레이어랑 가까워지면 달리기 stop
	if (1 == m_iStateCurNum)
	{
		m_pTransformCom->Go_Target(fTimeDelta, m_pPlayer->GetPlayerPos(), m_pNavigationCom);
	
		if (m_pTransformCom->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 2.5f))
		{
			m_iStateCurNum = 2;
		}
	}
	
	if (2 == m_iStateCurNum)
	{
		m_pModelCom->Set_AnimStop();
		m_eMonsterState = MONSTER_ATTACK;
		m_iStateCurNum = 0;
	}
}

void CMonster_Sword::motion_moveToInitPos(_float fTimeDelta)
{
	if (m_eMonsterState != MONSTER_MOVE_INIT)
		return;

	if (m_pTransformCom->Go_DistanceCheck(XMLoadFloat4(&m_pPosMove), 1.5f))
	{
		m_eMonsterState = MONSTER_IDLE;
		m_iStateCurNum = 1;
		return;
	}

	//걷기 없음
	//	달리기 1, 2

	if (0 == m_iStateCurNum)
	{
		m_pTransformCom->LookAt(XMLoadFloat4(&m_pPosMove));
		m_pModelCom->Set_AnimIndex(2, true);
		m_iStateCurNum = 1;
	}

	// initPos 랑 가까워지면 달리기 stop
	if (1 == m_iStateCurNum)
	{
		m_pTransformCom->Go_Target(fTimeDelta, XMLoadFloat4(&m_pPosMove), m_pNavigationCom);

		if (m_pTransformCom->Go_DistanceCheck(XMLoadFloat4(&m_pPosMove), 1.f))
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

void CMonster_Sword::motion_attack()
{
	if (m_eMonsterState != MONSTER_ATTACK)
		return;

	if (m_iStateCurNum == 0)
	{
		_uint tmp = rand() % 2;
		m_pModelCom->Set_AnimIndex(13 + tmp, false);	// 13 or 14
		m_iStateCurNum = 1;
	}

	if (1 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		motion_reset();
	}
}

void CMonster_Sword::motion_hit()
{
	if (m_eMonsterState != MONSTER_HIT)
		return;

	if(m_iStateCurNum == 0)
	{
		m_pModelCom->Set_AnimIndex(4, false);	// 10
		m_iStateCurNum = 1;
	}

	if (1 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		motion_reset();
	}
}

void CMonster_Sword::motion_idle()
{
	if (m_eMonsterState != MONSTER_IDLE)
		return;

	if(m_iStateCurNum == 1)
	{
		m_pModelCom->Set_AnimIndex(0, true);
		m_iStateCurNum = 0;
	}

}

void CMonster_Sword::motion_reset()
{
	m_iStateCurNum = 1;
	m_eMonsterState = MONSTER_IDLE;
}

void CMonster_Sword::CheckColl()
{
	if (m_bCol || m_eMonsterState != MONSTER_ATTACK)
		return;

	CPlayer* pTargetObject = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
	if (nullptr == pTargetObject)
		return;
	
	CCollider* pTargetCollider = pTargetObject->GetPlayerColl();
	if (nullptr == pTargetCollider)
		return;
	
	if (m_pColliderCom_Weapon->Intersect(pTargetCollider))
	{
		pTargetObject->SetDamaged(m_fAttack);
		m_bCol = true;
	}
}

CMonster_Sword * CMonster_Sword::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster_Sword*		pInstance = new CMonster_Sword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Sword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonster_Sword::Clone(void * pArg)
{
	CMonster_Sword*		pInstance = new CMonster_Sword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Sword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMonster_Sword::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pColliderCom_Weapon);
}
