#include "Player_LevelBoss.h"

#include "GameInstance.h"
#include "Boss.h"
#include "Boss_65_Body.h"

CPlayer_LevelBoss::CPlayer_LevelBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPlayer { pDevice, pContext }
{
}

CPlayer_LevelBoss::CPlayer_LevelBoss(const CPlayer_LevelBoss & rhs)
	: CPlayer( rhs )
{
}

HRESULT CPlayer_LevelBoss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_LevelBoss::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC*	Desc = static_cast<CTransform::TRANSFORM_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_LevelBoss::PriorityTick(_float fTimeDelta)
{
	__super::PriorityTick(fTimeDelta);
}

void CPlayer_LevelBoss::Tick(_float fTimeDelta)
{
	_vector curPos = GetPlayerPos();

	if ((XMVectorGetZ(curPos) >= 45 && XMVectorGetZ(curPos) <= 46.5))
	{
		CBoss* pBoss = dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0));
		if (pBoss->GetBossCurPtrnState() == BOSS_INTRO_WAIT)
			pBoss->SetCurBossState(BOSS_INTRO);
	}

	__super::Tick(fTimeDelta);
}

void CPlayer_LevelBoss::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

CPlayer_LevelBoss * CPlayer_LevelBoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_LevelBoss*		pInstance = new CPlayer_LevelBoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_LevelBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer_LevelBoss::Clone(void * pArg)
{
	CPlayer_LevelBoss*		pInstance = new CPlayer_LevelBoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_LevelBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_LevelBoss::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pWeaponMgr);
}
