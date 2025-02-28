#include "Player.h"

#include "GameInstance.h"
#include "Model.h"

#include "Level.h"
#include "Player_Body.h"
#include "Camera_Player.h"
#include "Player_Effect_Dash.h"
#include "UI_PlayerDamaged.h"
#include "Boss.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject( rhs )
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC*	Desc = static_cast<CTransform::TRANSFORM_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Desc->vInitPos.x, Desc->vInitPos.y, Desc->vInitPos.z, Desc->vInitPos.w));

	m_iHp = 100;
	m_iMaxHp = 100;
	m_fShield = 100.f;
	m_fMaxShield = 100.f;

	return S_OK;
}

void CPlayer::PriorityTick(_float fTimeDelta)
{
	ShieldCharge(fTimeDelta);

	if(GET_CURLEVEL == LEVEL_STAGE1_BOSS)
	{
		_vector curPos = GetPlayerPos();

		if ((XMVectorGetZ(curPos) >= 45 && XMVectorGetZ(curPos) <= 46.5))
		{
			CBoss* pBoss = dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0));
			if (pBoss->GetBossCurPtrnState() == BOSS_INTRO_WAIT)
			{
				pBoss->SetCurBossState(BOSS_INTRO);
			}
		}
	}

	if (m_iHp <= 0.f)
	{
		if(m_tPlayerState != DIE)
		{
			m_tPlayerState = DIE;
			PLAYONLY(L"PlayerDie");
			dynamic_cast<CPlayer_Body*>(m_pBody)->SaveCurMatix();
		}
		m_pBody->PriorityTick(fTimeDelta);

		if (g_bCoin >= 100 && KEY_PUSH(DIK_Y))
		{
			SetAlive();
			g_bCoin -= 100;
			m_tPlayerState = GODMODE;
			PLAY(L"PlayerReAlive", false);
		}
	}
	else
	{
		for (auto& pPartObj : m_Parts)
			pPartObj->PriorityTick(fTimeDelta);
	}

	if (m_tPlayerState == GODMODE)
	{
		m_fGodTime += fTimeDelta;
		if (m_fGodTime >= 3.f)
		{
			m_fGodTime = 0.f;
			m_tPlayerState = STOP;
			dynamic_cast<CPlayer_Body*>(m_pBody)->SetSavedMatix();
		}
	}

	if (m_bKnockBack)
		return;


	if(m_tPlayerSkill == DASH)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime <= .05f)
			m_fDashSpeed = 40.f;
		else
		{
			m_fDelayTime = 0.f;
			m_fDashSpeed = 1.f;
			m_tPlayerSkill = P_SKILL_NONE;
		}
	}

	KeyInput(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_iHp <= 0.f)
		m_pBody->Tick(fTimeDelta);
	else
	{
		for (auto& pPartObj : m_Parts)
			pPartObj->Tick(fTimeDelta);
	}

	m_fAttack = m_pWeaponMgr->GetCurWeaponAttack();

	Jump(fTimeDelta);
	KnockBack(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::LateTick(_float fTimeDelta)
{
	if (m_iHp <= 0.f)
		m_pBody->LateTick(fTimeDelta);
	else
	{
		for (auto& pPartObj : m_Parts)
			pPartObj->LateTick(fTimeDelta);
	}

	if(!m_bJump)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { XMVectorGetX(GetPlayerPos()), m_pNavigationCom->GetHeight(GetPlayerPos(), m_pNavigationCom->GetCurrIdx()) + 1.3f, XMVectorGetZ(GetPlayerPos()) , 1.f});

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);


#ifdef _DEBUG
	/* 렌더러클래스에 디버그용 렌더그룹에 추가해주고 디퍼드단계가 아닌 위치에서 컴포넌트들을 그릴 수 있도록 하겠다. */
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	//m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* 몸. */
	/* 바디의 부모는 플레이어!! */
	CPartObject::PARTOBJ_DESC		BodyDesc{};
	
	/* 추가된 파트오브젝트가 부모를 기준으로 회전하거나 이동할 수 있다. */
	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.fSpeedPerSec = 5.f;
	BodyDesc.fRotationPerSec = 2.f;
	
	m_pBody = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Player"), &BodyDesc));
	if (nullptr == m_pBody)
		return E_FAIL;

	/* 무기. */
	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentTransform = m_pTransformCom;
	WeaponDesc.fSpeedPerSec = 0.f;
	WeaponDesc.fRotationPerSec = 0.f;
	WeaponDesc.eWeaponState = CWeapon::IDLE;
	WeaponDesc.m_pPlayer = this;
	
	CPartObject* pWeapon = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_WeaponMgr", &WeaponDesc));
	if (nullptr == pWeapon)
		return E_FAIL;
	
	m_pWeaponMgr = dynamic_cast<CPlayer_WeaponMgr*>(pWeapon);

	m_Parts.emplace_back(pWeapon);


	///* 총구 이펙트. */
	//BodyDesc.pParentTransform = m_pTransformCom;
	//BodyDesc.fSpeedPerSec = 0.f;
	//BodyDesc.fRotationPerSec = 0.5f;
	//
	//CPartObject* pEffect = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Player_Effect_Flame", &BodyDesc));
	//if (nullptr == pEffect)
	//	return E_FAIL;
	//
	//m_Parts.emplace_back(pEffect);


	/* 카메라 */
	CCamera_Player::PLAYER_CAMERA_DESC			CameraDesc{};
	
	CameraDesc.pParentTransform = m_pTransformCom;
	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.vEye = _float4(0.f, 3.f, -5.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.m_pPlayerNavigationCom = m_pNavigationCom;
	CameraDesc.m_pPlayer = this;
	
	CPartObject* pCam = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc));
	if (nullptr == pCam)
		return E_FAIL;

	m_pCam = dynamic_cast<CCamera_Player*>(pCam);
	m_Parts.emplace_back(pCam);

	return S_OK;
}

HRESULT CPlayer::Ready_Components(void* pArg)
{
	/* 플레이어의 모든 파츠를 다 포함하여 충돌을 하고자한다. */

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC		NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = 0;
	//NavigationDesc.iCurrentCellIndex = 32;

	LEVEL currLevel = reinterpret_cast<PLAYER_DESC*>(pArg)->eLevel;

	if (currLevel == LEVEL_STAGE1)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Prototype_Component_Navigation_Stage1"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}
	if (currLevel == LEVEL_STAGE2)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_STAGE2, TEXT("Prototype_Component_Navigation_Stage2"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}
	if (currLevel == LEVEL_STAGE1_BOSS)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Navigation_StageBoss"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingDesc{};

	BoundingDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	BoundingDesc.vCenter = _float3(0.f, -.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	return S_OK;
}

void CPlayer::Jump(_float fTimeDelta)
{
	if (!m_bJump)
		return;

	m_fJumpPower = m_bKnockBack ? 2.f : 1.5f;

	_float4 fPos;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fJumpTime += fTimeDelta * 0.8f;
	
	fPos.y += (m_fJumpPower * m_fJumpTime) - (m_fGravity * m_fJumpTime * m_fJumpTime * 0.5f);

	if (fPos.y <= m_pNavigationCom->GetHeight(GetPlayerPos(), m_pNavigationCom->GetCurrIdx()) + 1.3f)
	{
		fPos.y = m_pNavigationCom->GetHeight(GetPlayerPos(), m_pNavigationCom->GetCurrIdx()) + 1.3f;
		m_fJumpTime = 0.f;
		m_bJump = false;
		m_bKnockBack = false;
	}
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&fPos));
}

void CPlayer::KnockBack(_float fTimeDelta)
{
	if (!m_bKnockBack)
		return;

	m_bJump = true;
	m_pTransformCom->Go_Straight(fTimeDelta * (-2.5f), m_pNavigationCom);

}

void CPlayer::KeyInput(float fTimeDelta)
{
	if (m_tPlayerState == GODMODE || m_tPlayerState == DIE)
		return;

	if (KEY_HOLD(DIK_W))
	{
		if (!m_bMoveAuto)
			m_pTransformCom->Go_Straight(fTimeDelta * m_fDashSpeed, m_pNavigationCom);
	}
	if (KEY_HOLD(DIK_S))
	{
 		m_pTransformCom->Go_Backward(fTimeDelta * m_fDashSpeed, m_pNavigationCom);
	}
	if (KEY_HOLD(DIK_A))
	{
		m_pTransformCom->Go_Left(fTimeDelta * m_fDashSpeed, m_pNavigationCom);
	}
	if (KEY_HOLD(DIK_D))
	{
		m_pTransformCom->Go_Right(fTimeDelta * m_fDashSpeed, m_pNavigationCom);
	}
	if (KEY_PUSH(DIK_E))
	{
		CTransform::TRANSFORM_DESC		transDesc{};

		transDesc.worldMat = *m_pTransformCom->Get_WorldFloat4x4();
		transDesc.fSpeedPerSec = 100.f;
		transDesc.fRotationPerSec = .5f;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Player_PosionBall", L"Prototype_GameObject_PlayerPosion", &transDesc)))
			MSG_BOX(L"Create Stone Failed ! ");
	}
	if (KEY_PUSH(DIK_SPACE))
		DoJump(true);

	if (KEY_PUSH(DIK_LSHIFT))
	{
		PLAY(L"heroDash", false);
		m_tPlayerSkill = DASH;
		dynamic_cast<CPlayer_Effect_Dash*>(GET_LIST(LEVEL_STATIC, L"Layer_Effect")->front())->ShowDashEffect();
	}
}

void CPlayer::SetDamaged(_float iTargetAtt)
{
	if (m_tPlayerState == GODMODE || m_tPlayerState == DIE)
		return;

	m_tPlayerState = DAMAGED;

	Attacked(iTargetAtt);
	PLAY(L"PlayerDamaged", false);
	dynamic_cast<CUI_PlayerDamaged*>(GET_LIST(LEVEL_STATIC, L"Player_UI_Damaged")->front())->SetDamagedUI();
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pWeaponMgr);

	for (auto& pPartObj : m_Parts)
		Safe_Release(pPartObj);

	Safe_Release(m_pBody);
	Safe_Release(m_pCam);
}
