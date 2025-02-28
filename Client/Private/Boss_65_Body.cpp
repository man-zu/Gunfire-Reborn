#include "Boss_65_Body.h"

#include "GameInstance.h"
#include "Player.h"
#include "Boss.h"
#include "GameObject.h"
#include "UI_Crosshair.h"

CBoss_65_Body::CBoss_65_Body(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext } 
{
}

CBoss_65_Body::CBoss_65_Body(const CBoss_65_Body & rhs)
	: CPartObject(rhs)
{
}

const _float4x4 * CBoss_65_Body::Get_BoneMatrixPtr(const _char * pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);	
}

HRESULT CBoss_65_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_65_Body::Initialize(void * pArg)
{
	BODY_DESC*		pDesc = static_cast<BODY_DESC*>(pArg);

	m_eBossState = pDesc->tBossState;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0, true);

	/* 최초 위치 설정 */
	m_PosMove = { 69.f, -90.f, 135.5f, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_PosMove));
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	return S_OK;
}

void CBoss_65_Body::PriorityTick(_float fTimeDelta)
{
	
}

void CBoss_65_Body::Tick(_float fTimeDelta)
{
/*
	[0]	 "(3501_Stand_InCombat)--3501_Stand_InCombat" m_fDuration=...}
	[1]	 "(3501_Walk_F)--3501_Walk_F" m_fDuration=2.00000000 ...}
	[2]	 "(3501_Walk_R)--3501_Walk_R" m_fDuration=2.00000000 ...}
	[3]	 "(3501_Walk_B)--3501_Walk_B" m_fDuration=2.00000000 ...}
	[4]	 "(3501_Walk_L)--3501_Walk_L" m_fDuration=2.00000000 ...}
	[5]	 "(3501_Run)--3501_Run" m_fDuration=0.933300018 m_fTickPerSecond=...}
	[6]	 "3501_die++++X" m_fDuration=9.69999981 m_fTickPerSecond=...}
	[7]	 "(3501_Jump)--3501_Jump" m_fDuration=1.39999998 m_fTickPerSecond=...}
	[8]	 "(3501_Jump)--3501_Land" m_fDuration=2.23333335 m_fTickPerSecond=...}
	[9]	 "(3501_Hit_Head)--3501_Hit_Head" m_fDuration=1.39999998 ...}
	[10] "(3501_Hit_Body)--3501_Hit_Body" m_fDuration=1.26670003 ...}
	[11] "3501_Summon" m_fDuration=4.09999990 m_fTickPerSecond=...}
	[12] "(3501_Attack)--3501_Attack" m_fDuration=3.59999990 ...}
	[13] "(3501_Fire02)--3501_Fire02" m_fDuration=3.66669989 ...}
	[14] "(3501_attack02)--3501_attack02" m_fDuration=2.70000005 ...}
	[15] "(3501_Attack03)--3501_FireReady" m_fDuration=0.833299994 ...}
	[16] "(3501_Attack03)--3501_Fire" m_fDuration=0.566699982 ...}
	[17] "(3501_Attack03)--3501_end" m_fDuration=0.833299994 ...}
	
	달리고 주먹에서 빨간 불? 쏘기
	주먹에서 특정 장판위치에 돌던지는거
	점프해서 내려찍기
	달려서 주먹으로 바닥 내려찍기?
	
	거점에서 보호막 회복

*/

	motion_intro(fTimeDelta);
	motion_idle(fTimeDelta);
	motion_move(fTimeDelta);
	motion_attack_run_jump(fTimeDelta);
	motion_attack_throw_rock(fTimeDelta);
	motion_attack_run_punch(fTimeDelta);
	motion_attack_run_shot(fTimeDelta);
	motion_damaged(fTimeDelta);
	motion_healing(fTimeDelta);
	motion_die();

	m_pModelCom->Play_Animation(fTimeDelta);
	
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	m_pColliderCom[0]->Update(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bip001 HeadNub")) * XMLoadFloat4x4(&m_WorldMatrix));
	m_pColliderCom[1]->Update(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bip001 L Finger0Nub")) * XMLoadFloat4x4(&m_WorldMatrix));
	m_pColliderCom[2]->Update(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("3501_hand_R_Bone020")) * XMLoadFloat4x4(&m_WorldMatrix));
	m_pColliderCom[3]->Update(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bip001 Spine1")) * XMLoadFloat4x4(&m_WorldMatrix));

	CheckColl();
}

void CBoss_65_Body::LateTick(_float fTimeDelta)
{

#ifdef _DEBUG
	for(int i=0;i<4;++i)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom[i]);
	}
#endif
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_SHADOWGAMEOBJECT, this);
}

HRESULT CBoss_65_Body::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CBoss_65_Body::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	/* 광원기준으로 표현된 뷰, 투영행렬이다. (카메라기준x) */
	_float4x4		LightViewMatrix, LightProjMatrix;

	XMStoreFloat4x4(&LightViewMatrix, XMMatrixLookAtLH(XMVectorSet(-10.f, 30.f, -10.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&LightProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		if (FAILED(m_pModelCom->Bind_Buffers(i)))
			return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CBoss_65_Body::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Monster_Boss_65"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	CBounding_OBB::BOUNDING_OBB_DESC	BoundingDesc{};

	BoundingDesc.vExtents = _float3(0.5f, 0.7f, .5f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRotation = _float3(0.f, 0.0f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider0"), reinterpret_cast<CComponent**>(&m_pColliderCom[0]), &BoundingDesc)))
		return E_FAIL;


	BoundingDesc.vExtents = _float3(.8f, .8f, 1.2f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRotation = _float3(0.f, 0.0f, -0.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider1"), reinterpret_cast<CComponent**>(&m_pColliderCom[1]), &BoundingDesc)))
		return E_FAIL;

	BoundingDesc.vExtents = _float3(1.2f, .8f, .8f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRotation = _float3(0.f, 0.0f, -0.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider2"), reinterpret_cast<CComponent**>(&m_pColliderCom[2]), &BoundingDesc)))
		return E_FAIL;


	BoundingDesc.vExtents = _float3(2.2f, .9f, .9f);
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
	BoundingDesc.vRotation = _float3(0.f, 0.0f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider3"), reinterpret_cast<CComponent**>(&m_pColliderCom[3]), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

void CBoss_65_Body::CheckColl()
{
}

HRESULT CBoss_65_Body::Bind_ShaderResources()
{
	_float4x4 mat, matInv;
	XMStoreFloat4x4(&mat, XMMatrixIdentity());
	XMStoreFloat4((_float4*)&mat.m[3][0], { 69.f, -90.f, 135.5f, 1.f });
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

	return S_OK;
}

void CBoss_65_Body::motion_intro(_float fTimeDelta)
{
	if (m_eBossState != BOSS_INTRO)
		return;

	if (0 == m_iStateCurNum)
	{
		m_iStateCurNum = 1;
		m_pModelCom->Set_AnimIndex(11, false);
	}

	if (m_iStateCurNum == 1)
	{
		m_fDelayTime += fTimeDelta;
		if (m_fDelayTime >= 2.5f)
		{
			PLAYONLY(L"BossArrr1");
			m_iStateCurNum = 2;
		}
	}

	if (2 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_iStateCurNum = 3;
		m_pModelCom->Set_AnimIndex(12, false);
		dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0))->SetJumpFromRock();
	}

	if( 3 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_eBossState = BOSS_IDLE;
		m_iStateCurNum = 0;
		m_fDelayTime = 0.f;
	}


}

void CBoss_65_Body::motion_idle(_float fTimeDelta)
{
	if (m_eBossState != BOSS_IDLE)
		return;

	if (m_eBossNextState == BOSS_HEALING)
	{
		m_eBossState = BOSS_HEALING;
		return;
	}

	if (!m_pParentTransform->TurnTargetYN({ 0.f, 1.f, 0.f }, fTimeDelta * 2.f, m_pPlayer->GetPlayerTransform()))
		doNextPattern();
	else
		m_eBossState = BOSS_MOVE;
}

void CBoss_65_Body::motion_move(_float fTimeDelta)
{
	if (m_eBossState != BOSS_MOVE)
		return;

	if (0 == m_iStateCurNum)
	{
		PLAYONLY(L"BossArrr3");
		m_pModelCom->Set_AnimIndex(5, true);
		m_iStateCurNum = 1;
	}

	m_pParentTransform->Go_Straight(fTimeDelta * 1.2f);
	if (m_pParentTransform->TurnTarget({ 0.f, 1.f, 0.f }, fTimeDelta * 1.2f, m_pPlayer->GetPlayerTransform()))
	{

		doNextPattern();
		m_fDelayTime = 0.f;
		m_iStateCurNum = 0;
		return;
	}
}

void CBoss_65_Body::doNextPattern()
{
	switch (m_eBossLastPtn)
	{
	case BOSS_ATTACK1:
		m_eBossLastPtn = BOSS_ATTACK2;
		m_eBossState = BOSS_ATTACK2;
		break;
	case BOSS_ATTACK2:
		m_eBossLastPtn = BOSS_ATTACK3;
		m_eBossState = BOSS_ATTACK3;
		break;
	case BOSS_ATTACK3:
		m_eBossLastPtn = BOSS_ATTACK4;
		m_eBossState = BOSS_ATTACK4;
		break;
	case BOSS_ATTACK4:
		m_eBossLastPtn = BOSS_ATTACK1;
		m_eBossState = BOSS_ATTACK1;
		break;
	case BOSS_END:
		m_eBossLastPtn = BOSS_ATTACK1;
		m_eBossState = BOSS_ATTACK1;
	}
}

void CBoss_65_Body::motion_attack_throw_rock(_float fTimeDelta)
{
	// 달려오다가 돌 던지기
	if (m_eBossState != BOSS_ATTACK2)
		return;

	if (0 == m_iStateCurNum)
	{
		m_pParentTransform->LookAt(m_pPlayer->GetPlayerPos());
		m_pModelCom->Set_AnimIndex(5, true);
		m_iStateCurNum = 1;
	}

	// 플레이어랑 가까워지면 달리기 stop
	if (1 == m_iStateCurNum)
	{
		m_pParentTransform->Go_Target(fTimeDelta * 1.2f, m_pPlayer->GetPlayerPos());

		if (m_pParentTransform->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 28.f))
			m_iStateCurNum = 2;
	}

	if (2 == m_iStateCurNum)
	{
		m_iStateCurNum = 3;
		m_pModelCom->Set_AnimIndex(13, false);

		// ThrowStone 생성
		if (!m_bCreateBool)
		{
			m_bCreateBool = true;
			dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0))->CreateThrowStone();
		}
	}

	if (3 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_eBossState = BOSS_IDLE;
		m_bCreateBool = false;
		m_iStateCurNum = 0;
	}
}

void CBoss_65_Body::motion_attack_run_jump(_float fTimeDelta)
{
	// 점프해서 내려찍는 모션
	if (m_eBossState != BOSS_ATTACK4)
		return;

	if (0 == m_iStateCurNum)
	{
		m_pParentTransform->LookAt(m_pPlayer->GetPlayerPos());
		m_pModelCom->Set_AnimIndex(5, true);
		m_iStateCurNum = 1;
	}

	// 달리기
	if (1 == m_iStateCurNum)
	{

		m_iStateCurNum = 2;
	}

	// 점프
	if (2 == m_iStateCurNum)
	{
		m_pParentTransform->Go_Target(fTimeDelta, m_pPlayer->GetPlayerPos());

		if (m_pParentTransform->Go_DistanceCheck(XMLoadFloat4(&m_PosTarget), 18.f))
		{
			m_iStateCurNum = 3;
		}
	}

	/* TargetPos Ring Effect */
	if(3 > m_iStateCurNum)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime >= .4f)
		{
			m_fDelayTime = 0.f;

			if (m_PosTarget.x == 0.f && m_PosTarget.y == 0.f)
				XMStoreFloat4(&m_PosTarget, m_pPlayer->GetPlayerPos());

			dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0))->CreateTargetRing(m_PosTarget);
		}
		return;
	}

	m_fDelayTime += fTimeDelta;
	
	if (3 == m_iStateCurNum)
	{
		m_pModelCom->Set_AnimIndex(7, false);
		m_iStateCurNum = 4;
	}
	else if(4 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_pModelCom->Set_AnimIndex(8, false);
		m_iStateCurNum = 5;
	}

	if(5 == m_iStateCurNum && m_fDelayTime >= .8f)
	{
		dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0))->CreateJumpStone();
		m_iStateCurNum = 6;
	}
	
	if (6 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		doNextPattern();
		m_fDelayTime = 0.f;
		m_iStateCurNum = 0;
		m_PosTarget = { 0.f, 0.f, 0.f, 1.f };
	}

}

void CBoss_65_Body::motion_attack_run_punch(_float fTimeDelta)
{
	// 점프해서 내려찍는 모션
	if (m_eBossState != BOSS_ATTACK1)
		return;

	if (0 == m_iStateCurNum)
	{
		PLAYONLY(L"BossArrr4");
		m_iStateCurNum = 1;
	}
	if (1 == m_iStateCurNum)
	{
		m_pParentTransform->LookAt(m_pPlayer->GetPlayerPos());
		m_pModelCom->Set_AnimIndex(5, true);
		m_iStateCurNum = 2;
	}

	// 플레이어랑 가까워지면 달리기 stop
	if (2 == m_iStateCurNum)
	{
		m_pParentTransform->Go_Target(fTimeDelta * 1.3f, m_pPlayer->GetPlayerPos());

		if(m_pParentTransform->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 9.f))
			m_iStateCurNum = 3;
	}

	if (3 == m_iStateCurNum)
	{
		m_iStateCurNum = 4;
		m_pModelCom->Set_AnimIndex(14, false);
	}

	if(4 == m_iStateCurNum)
	{
		m_fDelayTime += fTimeDelta;

		if(m_fDelayTime >= 0.8f)
		{
			dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0))->CreatePunchFloor();
			m_iStateCurNum = 5;
		}
	}

	if (5 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_eBossState = BOSS_IDLE;
		m_iStateCurNum = 0;
		m_fDelayTime = 0;
	}
}

void CBoss_65_Body::motion_attack_run_shot(_float fTimeDelta)
{
	// 플레이어로 뛰어오고 주먹에서 총 쏘기
	if (m_eBossState != BOSS_ATTACK3)
		return;

	if (0 == m_iStateCurNum)
	{
		m_pParentTransform->LookAt(m_pPlayer->GetPlayerPos());
		m_pModelCom->Set_AnimIndex(5, true);
		m_iStateCurNum = 1;
	}

	// 플레이어랑 가까워지면 애니메이션 stop
	if (1 == m_iStateCurNum)
	{
		m_pParentTransform->Go_Target(fTimeDelta, m_pPlayer->GetPlayerPos());

		if (m_pParentTransform->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 28.f))
		{
			m_iStateCurNum = 2;
			m_pModelCom->Set_AnimIndex(15, false);
		}
	}

	if (2 == m_iStateCurNum)
	{
		if (m_pModelCom->Get_Finished())
		{
			dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0))->CreateShot();
		
			m_pModelCom->Set_AnimIndex(16, false);
			m_iShotCnt++;
		}
		
		if(m_iShotCnt == 3)
			m_iStateCurNum = 3;
	}

	if (3 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_eBossState = BOSS_IDLE;
		m_iStateCurNum = 0;
		m_iShotCnt = 0;
	}

}

void CBoss_65_Body::motion_damaged(_float fTimeDelta)
{
	// 플레이어로 뛰어오고 주먹에서 총 쏘기
	if (m_eBossState != BOSS_DAMAGED)
		return;

	if(0 == m_iStateCurNum)
	{
		m_iStateCurNum = 1;
		m_pModelCom->Set_AnimIndex(9, false);
	}

	if (1 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_eBossState = BOSS_IDLE;
		m_iStateCurNum = 0;
		m_iShotCnt = 0;
	}
}

void CBoss_65_Body::motion_die()
{
	if (m_eBossState != BOSS_DIE)
		return;

	if(m_iStateCurNum <= 99)
	{
		m_pModelCom->Set_AnimStop();
		m_iStateCurNum = 99;
		PLAYONLY(L"BossDie");
	}

	if (m_iStateCurNum == 99)
	{
		PLAYONLY(L"BossDieSound");
		m_pModelCom->Set_AnimIndex(6, false);
		m_iStateCurNum = 100;
	}

	if(m_iStateCurNum == 100 && m_pModelCom->Get_Finished())
	{
		list<CGameObject*>* CrosshairUi = GET_LIST(LEVEL_STATIC, L"Layer_Crosshair");
		dynamic_cast<CUI_Crosshair*>(CrosshairUi->front())->SetHide(true);

		m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_Victory", L"Prototype_GameObject_UI_Victory");
		m_iStateCurNum = 101;

		list<CGameObject*>* BossUiObjList = GET_LIST(LEVEL_STAGE1_BOSS, L"Layer_UI_Boss_Icon");
		for (list<CGameObject*>::iterator iter = BossUiObjList->begin();
			iter != BossUiObjList->end(); ++iter)
		{
			(*iter)->Set_Dead(true);
		}

		BossUiObjList = GET_LIST(LEVEL_STAGE1_BOSS, L"Layer_UI_Boss_Bar");
		for (list<CGameObject*>::iterator iter = BossUiObjList->begin();
			iter != BossUiObjList->end(); ++iter)
		{
			(*iter)->Set_Dead(true);
		}
	}
}

void CBoss_65_Body::motion_healing(_float fTimeDelta)
{
	if (m_eBossState != BOSS_HEALING)
		return;

	m_fDelayTime += fTimeDelta;

	if (m_iStateCurNum == 0)
	{
		m_pModelCom->Set_AnimIndex(5, true);
		m_iStateCurNum = 1;
	}

	// 돌산과 가까워지면 달리기 stop
	if (1 == m_iStateCurNum)
	{
		_float4 m_pPosMoveTarget = { 50.f, 1.f, 102.f, 1.f };

		m_pParentTransform->Go_Target(fTimeDelta * 1.2f, XMLoadFloat4(&m_pPosMoveTarget));

		if (m_pParentTransform->Go_DistanceCheck(XMLoadFloat4(&m_pPosMoveTarget), 5.f))
		{
			m_pModelCom->Set_AnimIndex(7, false);
			m_iStateCurNum = 2;
			m_fDelayTime = 0;
		}
	}

	// 돌산 위로 점프
	if (m_iStateCurNum == 2)
	{
		if(m_fDelayTime >= .75f)
		{
			if (m_fDelayTime <= .9f)
				dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0))->SetJumpUp();

			if (m_pModelCom->Get_Finished())
				m_pModelCom->Set_AnimIndex(8, false);

			_float4 m_pPosMoveTarget = { 54.6f, 12.f, 120.f, 1.f };

			m_pParentTransform->Go_Target(fTimeDelta * 3.22f, XMLoadFloat4(&m_pPosMoveTarget));

			if (m_pParentTransform->Go_DistanceCheck(XMLoadFloat4(&m_pPosMoveTarget), 1.f))
			{
				m_pModelCom->Set_AnimIndex(7, false);
				m_fDelayTime = 0.f;
				m_iStateCurNum = 3;
			}
		}
	}

	// 돌산 위에서 정면방향으로 회전
	if (m_iStateCurNum == 3)
	{
		if (m_pModelCom->Get_Finished())
			m_pModelCom->Set_AnimIndex(8, false);
		if (m_fDelayTime <= 1.f)
		{
		}
		else if (m_fDelayTime <= 1.6f)
			m_pParentTransform->Turn({ 0.f, 1.f, 0.f }, fTimeDelta * 1.5f);
		else if(m_fDelayTime >= 2.6f)
		{
			m_fDelayTime = 0.f;
			m_iStateCurNum = 4;
			m_pModelCom->Set_AnimIndex(11, false);
		}
	}

	// 힐링모션
	if (m_iStateCurNum == 4)
	{
		if (dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0))->BossHealling(fTimeDelta))
			m_iStateCurNum = 5;
	}
	if (4 == m_iStateCurNum && m_fDelayTime >= 2.7f && !m_bSound)
	{
		PLAYONLY(L"BossArrr2");
		m_bSound = true;
	}

	// 다시 바닥으로 점프
	if (5 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_fDelayTime = 0.f;
		m_iStateCurNum = 6;
		m_pModelCom->Set_AnimIndex(12, false);
	}

	if (6 == m_iStateCurNum)
	{
		if (m_fDelayTime >= .2f && m_fDelayTime <= .4f)
		{
			m_iStateCurNum = 7;
			dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0))->SetJumpFromRock();
		}
	}

	// 공격패턴 다시 시작
	if (7 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_eBossNextState = BOSS_END;
		doNextPattern();
		m_fDelayTime = 0.f;
		m_iStateCurNum = 0;
	}

	return;
}

CBoss_65_Body * CBoss_65_Body::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_65_Body*		pInstance = new CBoss_65_Body(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_65_Body"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBoss_65_Body::Clone(void * pArg)
{
	CBoss_65_Body*		pInstance = new CBoss_65_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_65_Body"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBoss_65_Body::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

	for (auto& pColliderCom : m_pColliderCom)
		Safe_Release(pColliderCom);

}
