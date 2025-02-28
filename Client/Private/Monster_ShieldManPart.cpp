#include "Monster_ShieldManPart.h"

#include "GameInstance.h"
#include "Player.h"
#include "Effect_Common.h"
#include "Monster.h"

CMonster_ShieldManPart::CMonster_ShieldManPart(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CMonster_ShieldManPart::CMonster_ShieldManPart(const CMonster_ShieldManPart & rhs)
	: CPartObject(rhs)
{
}

HRESULT CMonster_ShieldManPart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_ShieldManPart::Initialize(void * pArg)
{
	m_eMonsterState = MONSTER_IDLE;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Navi(pArg)))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(3, true);	// 3

	/* 최초 위치 설정 */
	_float4 pPosMove = { -33.7f, -1.2f, -21.5f, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pPosMove));

	XMStoreFloat4(&m_pPosMove, m_pParentTransform->Get_State(CTransform::STATE_POSITION));

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	m_fAttack = 60.f;

	return S_OK;
}

void CMonster_ShieldManPart::PriorityTick(_float fTimeDelta)
{


}

void CMonster_ShieldManPart::Tick(_float fTimeDelta)
{
	/*
	+	[0] "(2008_Stand_InCombat)--2008_Stand_InCombat"
	+	[1] "(2008_Patrol)--2008_Patrol+
	+	[2] "(2008_Run)--2002_Run"
	+	[3] "(2008_Stand)--2008_Stand"
	+	[4] "(2008_Walk_F)--2008_Walk_F+
	+	[5] "(2008_Walk_R)--2008_Walk_R+
	+	[6] "(2008_Walk_B)--2008_Walk_B+
	+	[7] "(2008_Walk_L)--2008_Walk_L+
	+	[8] <문자열에 잘못된 문자>
	+	[9] <문자열에 잘못된 문자>
	+	[10] <문자열에 잘못된 문자>
	+	[11] <문자열에 잘못된 문자>
	+	[12] "(2123_Show)--2123_Show+
	+	[13] "(2008_Jump)--2008_JumpStart"
	+	[14] "(2008_Jump)--2008_JumpEnd"
	+	[15] "(2008_Hit_Head)--2008_Hit_Head"
	+	[16] "(2008_attack01)--2008_attack01+
	+	[17] "(2008_Shied_Fire)--2008_Shied_Fire+
	+	[18] "(2008_attack02)--2008_attack02"
	
		IDLE 0
		걷기 4
		달리기 2
		공격
		 - 검 가로슥삭 16
		 - 검 세로솩 17
		 - 방패로 때리기 18
		피격 15
	
	*/
	if (m_pPlayer->GetPlayerState() == CPlayer::DIE
		&& (m_eMonsterState != MONSTER_MOVE_INIT && !m_pTransformCom->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 10.5f)))
	{
		m_eMonsterState = MONSTER_MOVE_INIT;
		m_iStateCurNum = 0;
	}

	if (m_eMonsterState == MONSTER_HIT)
		motion_hit();
	if (m_eMonsterState != MONSTER_HIT)
	{
		motion_moveToPlayer(fTimeDelta);
		motion_moveToInitPos(fTimeDelta);
		motion_attack();
		motion_idle();
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	/* SET NAVI HEIGHT */
	_float4 vCurPos;
	XMStoreFloat4(&vCurPos, m_pParentTransform->Get_State(CTransform::STATE_POSITION));
	_float fNaviHeight = m_pNavigationCom->GetHeight(m_pParentTransform->Get_State(CTransform::STATE_POSITION), m_pNavigationCom->GetCurrIdx());
	vCurPos.y = 1.2f + fNaviHeight;
	m_pParentTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vCurPos));

	/* UPDATE PART TRANSFORM */
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update(m_pParentTransform->Get_WorldMatrix());
	m_pCollider_Shield->Update(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bip001 L Finger1Nub")) * XMLoadFloat4x4(&m_WorldMatrix));
	m_pCollider_Weapon->Update(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bip001 R Finger1Nub")) * XMLoadFloat4x4(&m_WorldMatrix));

	CheckColl();
}

void CMonster_ShieldManPart::LateTick(_float fTimeDelta)
{

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pCollider_Shield);
	m_pGameInstance->Add_DebugComponent(m_pCollider_Weapon);
#endif

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_SHADOWGAMEOBJECT, this);
}

HRESULT CMonster_ShieldManPart::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)0, aiTextureType_DIFFUSE)))
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

HRESULT CMonster_ShieldManPart::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_shieldMan"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingAABBDesc{};
	
	BoundingAABBDesc.vExtents = _float3(.5f, 1.4f, .5f);
	BoundingAABBDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingAABBDesc)))
		return E_FAIL;

	/* For.Com_Collider_AABB */
	BoundingAABBDesc.vExtents = _float3(.55f, 1.0f, .2f);
	BoundingAABBDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_Shield"), reinterpret_cast<CComponent**>(&m_pCollider_Shield), &BoundingAABBDesc)))
		return E_FAIL;

	/* For.Com_Collider_AABB */
	BoundingAABBDesc.vExtents = _float3(.3f, .3f, .7f);
	BoundingAABBDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_Weapon"), reinterpret_cast<CComponent**>(&m_pCollider_Weapon), &BoundingAABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_ShieldManPart::Ready_Navi(void* pArg)
{
	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC		NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = static_cast<CMonster::MONSTER_DESC*>(pArg)->iCurrentCellIndex;

	if(static_cast<CMonster::MONSTER_DESC*>(pArg)->eLevel == LEVEL_STAGE1)
	{
		if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Prototype_Component_Navigation_Stage1"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}
	if(static_cast<CMonster::MONSTER_DESC*>(pArg)->eLevel == LEVEL_STAGE2)
	{
		if (FAILED(__super::Add_Component(LEVEL_STAGE2, TEXT("Prototype_Component_Navigation_Stage2"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_ShieldManPart::Bind_ShaderResources()
{
	_float4x4 mat, matInv;
	XMStoreFloat4x4(&mat, XMMatrixIdentity());
	XMStoreFloat4((_float4*)&mat.m[3][0], { -33.7f, -1.2f, -21.5f, 1.f });
	XMStoreFloat4x4(&matInv, XMMatrixInverse(nullptr, XMLoadFloat4x4(&mat)));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_partMatrix", &mat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_partMatrixInv", &matInv)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

void CMonster_ShieldManPart::motion_moveToPlayer(_float fTimeDelta)
{
	if ((m_eMonsterState == MONSTER_ATTACK || m_eMonsterState == MONSTER_MOVE_INIT)
		|| !m_pParentTransform->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 10.f))
		return;

	m_eMonsterState = MONSTER_MOVE_PLAYER;

	if (0 == m_iStateCurNum)
	{
		m_pParentTransform->LookAt(m_pPlayer->GetPlayerPos());
		m_pModelCom->Set_AnimIndex(4, true);	// 걷기
		m_iStateCurNum = 1;
	}

	// 플레이어랑 가까워지면 걷기 stop
	if (1 == m_iStateCurNum)
	{
		m_pParentTransform->Go_Target(fTimeDelta, m_pPlayer->GetPlayerPos(), m_pNavigationCom);

		if (m_pParentTransform->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 4.2f))
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

void CMonster_ShieldManPart::motion_moveToInitPos(_float fTimeDelta)
{
	if (m_eMonsterState != MONSTER_MOVE_INIT)
		return;

	if (m_pParentTransform->Go_DistanceCheck(XMLoadFloat4(&m_pPosMove), 1.5f))
	{
		m_eMonsterState = MONSTER_IDLE;
		m_iStateCurNum = 1;
		return;
	}

	if (0 == m_iStateCurNum)
	{
		m_pParentTransform->LookAt(XMLoadFloat4(&m_pPosMove));
		m_pModelCom->Set_AnimIndex(2, true);	// 달리기
		m_iStateCurNum = 1;
	}

	// 플레이어랑 가까워지면 걷기 stop
	if (1 == m_iStateCurNum)
	{
		m_pParentTransform->Go_Target(fTimeDelta * 1.5f, XMLoadFloat4(&m_pPosMove), m_pNavigationCom);

		if (m_pParentTransform->Go_DistanceCheck(XMLoadFloat4(&m_pPosMove), 3.f))
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

void CMonster_ShieldManPart::motion_attack()
{
	/*
		- 검 가로베기 16
		- 검 세로베기 17
		- 방패로 때리기 18
	*/
	if (m_eMonsterState != MONSTER_ATTACK)
		return;

	if (m_iStateCurNum == 0)
	{
		_uint tmp = rand() % 3;
		m_pModelCom->Set_AnimIndex(16 + tmp, false);	// 16, 17, 18
		m_iStateCurNum = 1;
	}

	if (1 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		motion_reset();
	}
}

void CMonster_ShieldManPart::motion_hit()
{
	if (m_eMonsterState != MONSTER_HIT)
		return;

	if (m_iStateCurNum == 0)
	{
		m_pModelCom->Set_AnimIndex(15, false);
		m_iStateCurNum = 1;
	}

	if (1 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		motion_reset();
	}
}


void CMonster_ShieldManPart::motion_idle()
{
	if (m_eMonsterState != MONSTER_IDLE)
		return;

	if (m_iStateCurNum == 1)
	{
		m_pModelCom->Set_AnimIndex(0, true);
		m_iStateCurNum = 0;
	}

}

void CMonster_ShieldManPart::motion_reset()
{
	m_iStateCurNum = 1;
	m_eMonsterState = MONSTER_IDLE;
}

void CMonster_ShieldManPart::CheckColl()
{
	CPlayer* pTargetObject = m_pPlayer;
	if (nullptr == pTargetObject)
		return;
	
	CCollider* pTargetCollider = pTargetObject->GetPlayerColl();
	if (nullptr == pTargetCollider)
		return;
	
	if (m_pCollider_Weapon->Intersect(pTargetCollider))
		pTargetObject->SetDamaged(m_fAttack);

}

CMonster_ShieldManPart * CMonster_ShieldManPart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster_ShieldManPart*		pInstance = new CMonster_ShieldManPart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_ShieldManPart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonster_ShieldManPart::Clone(void * pArg)
{
	CMonster_ShieldManPart*		pInstance = new CMonster_ShieldManPart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_ShieldManPart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMonster_ShieldManPart::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCollider_Shield);
	Safe_Release(m_pCollider_Weapon);
}
