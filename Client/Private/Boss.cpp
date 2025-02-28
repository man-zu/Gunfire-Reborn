#include "Boss.h"

#include "GameInstance.h"

#include "Boss_65_Body.h"
#include "Boss_Skill_ThrowStone.h"
#include "Effect_Common.h"
#include "Player.h"
#include "Monster.h"

CBoss::CBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CBoss::CBoss(const CBoss & rhs)
	: CGameObject( rhs )
{
}

HRESULT CBoss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC		Desc{};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 54.6f, 12.f, 113.6f, 1.f });
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(180));

	m_iHp = m_iMaxHp = 2000;
	m_fShield = m_fMaxShield = 1000.f; // 1000;
	m_fAttack = 50.f;

	return S_OK;
}

void CBoss::PriorityTick(_float fTimeDelta)
{
	if (m_iHp <= 0)
		m_pBody->SetCurBossState(BOSS_DIE);

	if (m_iHp < m_iMaxHp * 0.8f && !m_bChargeShield)
	{
		m_bChargeShield = true;
		m_pBody->SetNextBossState(BOSS_HEALING);
	}
	if (m_iHp < m_iMaxHp * 0.4f && !m_bChargeSeconeShield)
	{
		m_bChargeSeconeShield = true;
		m_pBody->SetNextBossState(BOSS_HEALING);
	}

	for (auto& pPartObj : m_Parts)
		pPartObj->PriorityTick(fTimeDelta);
	
}

void CBoss::Tick(_float fTimeDelta)
{
	if (BOSS_IDLE == m_pBody->GetCurBossState())
		m_fDelayTime = 0.f;

	BossJumpFromRock(fTimeDelta);

	for (auto& pPartObj : m_Parts)
		pPartObj->Tick(fTimeDelta);

	if (m_bJump)
		JumpDown(fTimeDelta);

	if (m_bJumpUp)
		JumpUp(fTimeDelta);

	CheckColl();
}

void CBoss::LateTick(_float fTimeDelta)
{
	for (auto& pPartObj : m_Parts)
		pPartObj->LateTick(fTimeDelta);

	if (!m_bJump && m_pBody->GetCurBossState() != BOSS_HEALING && m_pBody->GetCurBossState() != BOSS_INTRO && m_pBody->GetCurBossState() != BOSS_INTRO_WAIT)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { XMVectorGetX(GetBossPos()), 2.3f, XMVectorGetZ(GetBossPos()), 1.f });
	if (m_pBody->GetCurBossState() == BOSS_DIE)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { XMVectorGetX(GetBossPos()), 3.5f, XMVectorGetZ(GetBossPos()), 1.f });

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBoss::Render()
{
	return S_OK;
}

HRESULT CBoss::Ready_PartObjects()
{
	/* 몸. */
	/* 바디의 부모는 플레이어!! */
	CBoss_65_Body::BODY_DESC		BodyDesc{};
	
	/* 추가된 파트오브젝트가 부모를 기준으로 회전하거나 이동할 수 있다. */
	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.tBossState = BOSS_INTRO_WAIT;
	
	m_pBody = dynamic_cast<CBoss_65_Body*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Boss_65"), &BodyDesc));
	if (nullptr == m_pBody)
		return E_FAIL;

	m_Parts.emplace_back(m_pBody);

	return S_OK;
}

HRESULT CBoss::Ready_Components()
{
	/* 플레이어의 모든 파츠를 다 포함하여 충돌을 하고자한다. */
	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC		NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = 0;
	
	if (FAILED(__super::Add_Component(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Navigation_StageBoss"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::Bind_ShaderResources()
{
	return S_OK;
}

void CBoss::JumpDown(_float fTimeDelta)
{
	m_fJumpPower = 1.0f;

	_float4 fPos;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fJumpTime += fTimeDelta * 0.8f;
	
	fPos.y += (m_fJumpPower * m_fJumpTime) - (m_fGravity * m_fJumpTime * m_fJumpTime * 0.5f);
	
	if (fPos.y <= 2.2f)
	{
		m_fJumpTime = 0.f;
		m_bJump = false;
		fPos.y = 2.3f;
	}
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&fPos));
}

void CBoss::JumpUp(_float fTimeDelta)
{
	m_fJumpPower = 3.3f;

	_float4 fPos;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fJumpTime += fTimeDelta;

	fPos.y += (m_fJumpPower * m_fJumpTime) - (m_fGravity * m_fJumpTime * m_fJumpTime * 0.5f);

	if ((m_fJumpPower * m_fJumpTime) < (m_fGravity * m_fJumpTime * m_fJumpTime * 0.5f))
	{
		m_bFall = true;
	}

	if (fPos.y >= 11.8f && m_bFall)
	{
		m_bFall = false;
		m_fJumpTime = 0.f;
		m_bJumpUp = false;
		fPos.y = 12.f;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&fPos));

}

void CBoss::CreatePunchFloor()
{
	CTransform::TRANSFORM_DESC transDesc{};
	transDesc.worldMat = *m_pTransformCom->Get_WorldFloat4x4();

	m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_Boss_Effect_PunchFloor", L"Prototype_GameObject_Boss_Effect_Crack", &transDesc);
	m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_Boss_Effect_PunchFloor", L"Prototype_GameObject_Boss_Effect_CrackLight", &transDesc);
}

void CBoss::CreateThrowStone()
{
	PLAYONLY(L"ThrowStone");

	_vector bossPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CBoss_Skill_ThrowStone::THROWSTONE		ThrowStoneDesc{};

	ThrowStoneDesc.worldMat = *m_pTransformCom->Get_WorldFloat4x4();
	ThrowStoneDesc.fSpeedPerSec = 200.f;
	ThrowStoneDesc.fRotationPerSec = .5f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_Boss_Skill_ThrowStone", L"Prototype_GameObject_Boss_Skill_ThrowStone", &ThrowStoneDesc)))
		MSG_BOX(L"Create Stone Failed ! ");
}

void CBoss::CreateJumpStone()
{
	PLAYONLY(L"JumpStone");

	CEffect_Common::EFFECT_DESC effectDesc{};
	_float3 CurPos;
	XMStoreFloat3(&CurPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	effectDesc.vInitPos = { CurPos.x, CurPos.y, CurPos.z, 1.f};

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_Boss_Skill_JumpStone", L"Prototype_GameObject_Boss_Skill_JumpStone", &effectDesc)))
		MSG_BOX(L"Create Stone Failed ! ");

}

void CBoss::CreateTargetRing(_float4 PosTarget)
{
	m_pPosTarget = PosTarget;

	/* 효과 */
	CEffect_Common::EFFECT_DESC effectDesc{};
	effectDesc.vInitPos = m_pPosTarget;
	effectDesc.vInitPos.y = -.6f;

	FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_Effect", L"Prototype_GameObject_Boss_Effect_TargetRing", &effectDesc));
}

void CBoss::CreateShot()
{
	CTransform::TRANSFORM_DESC		Desc{};
	Desc.vInitPos = { XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION)),
					 XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + 3.f,
					 XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION)),
					 1.f };

	m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_Boss_Shot_Effect", L"Prototype_GameObject_Particle_Explosion", &Desc);

	/* 효과 */
	CEffect_Common::EFFECT_DESC effectDesc{};
	XMStoreFloat4(&effectDesc.vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	effectDesc.vInitPos.y += 5.f;
	effectDesc.pWorldTransform = *m_pTransformCom->Get_WorldFloat4x4();
	effectDesc.iImgCnt = 8;
	effectDesc.fSpeed = 1.5f;
	effectDesc.vScale = { 13.f, 13.f, 13.f };
	effectDesc.sTextureTag = L"Prototype_Component_Texture_Effect_Glow";
	effectDesc.MoveAmount = { 6.5f, 2.f, 1.f, 1.f };

	FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Effect", L"Prototype_Component_Effect_Common", &effectDesc));


	_vector bossPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	BOSSSKILL		SkillDesc{};

	SkillDesc.worldMat = *m_pTransformCom->Get_WorldFloat4x4();
	SkillDesc.fSpeedPerSec = 50.f;
	SkillDesc.fAttack = BOSS_SHOT_DAMAGE;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_Boss_Skill_Shot", L"Prototype_GameObject_Boss_Skill_Shot", &SkillDesc)))
		MSG_BOX(L"Create Stone Failed ! ");
}

_bool CBoss::BossHealling(_float fTimeDelta)
{
	if (m_fShield == m_fMaxShield)
		return true;

	/* BoomBoom 생성 */
	m_fDelayTime += fTimeDelta;

	if (m_fDelayTime >= 1.5f)
	{
		m_fDelayTime = 0.f;
		CreateBoom();
	}

	/* Shield bar 채우기 */
	if (m_fChargeShield == 0.f && m_fShield >= m_fChargeShield)
	{
		m_fDelayTime = 0.f;
		m_fChargeShield = m_fMaxShield;
	}

	m_fShield += 5.f;
	m_fChargeShield -= 5.f;

	return false;
}

void CBoss::BossJumpFromRock(_float fTimeDelta)
{
	if (!m_JumpFromRock)
		return;

	/* 12: [보스 육오 인트로] 바위에서 점프 모션 */
	if (!m_Once)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime >= .75f)
		{
			m_Once = true;
			m_bJump = true;
			m_fDelayTime = 0.f;
		}
	}
	if (m_bJump && 12 == m_pBody->Get_CurStateIdx())
		m_pTransformCom->Go_Straight(fTimeDelta * 4.2f);

	if (BOSS_HEALING != m_pBody->GetCurBossState() && 7 == m_pBody->Get_CurStateIdx())
	{
		BossJumpAttack(fTimeDelta, m_pPosTarget);
	}
}

void CBoss::BossJumpAttack(_float fTimeDelta, _float4 vTargetPos)
{
	m_fDelayTime += fTimeDelta;

	if(m_fDelayTime >= .5f)
		m_pTransformCom->Go_Target(fTimeDelta * 1.5f, XMLoadFloat4(&vTargetPos));

}

void CBoss::CreateBoom()
{

	CMonster::MONSTER_DESC dsec{};

	/* 1-1 */
	dsec.vInitPos = { 40.f, 0.5f, 85.f, 1.f };
	dsec.iCurrentCellIndex = 22;
	dsec.eLevel = LEVEL_STAGE1_BOSS;

	m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_Monster_Boom", L"Prototype_GameObject_Monster_Boom", &dsec);

	dsec.vInitPos = { 65.f, 0.5f, 85.f, 1.f };
	m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_Monster_Boom", L"Prototype_GameObject_Monster_Boom", &dsec);
}

CBoss * CBoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss*		pInstance = new CBoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBoss::Clone(void * pArg)
{
	CBoss*		pInstance = new CBoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);

	for (auto& pPartObj : m_Parts)
		Safe_Release(pPartObj);
}
