#include "Weapon.h"

#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Player_Bullet.h"
#include "Player_BlueBullet.h"
#include "Player_Basic.h"
#include "Player_BlueEye.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: CPartObject(rhs)
{
}

HRESULT CWeapon::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_eState = pDesc->eWeaponState;
	m_pPlayer = pDesc->m_pPlayer;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::InitializeChild(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	//m_pBoneMatrix = pDesc->pBoneMatrix;
	m_eState = pDesc->eWeaponState;
	m_pPlayer = pDesc->m_pPlayer;
	eWeaponType = pDesc->eWeaponType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CWeapon::PriorityTick(_float fTimeDelta)
{
}

void CWeapon::Tick(_float fTimeDelta)
{
}

void CWeapon::LateTick(_float fTimeDelta)
{
}

HRESULT CWeapon::Render()
{
	return S_OK;
}

void CWeapon::MotionWeaponChange(_float fTimeDelta)
{
	if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < -3.f)
	{
		m_eState = HIDE;
		m_bUseYN = false;
		m_fTimeAcc = 0.f;
	}

	m_fTimeAcc += fTimeDelta;
	m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, fTimeDelta * .8f);
	m_pTransformCom->Go_Down(fTimeDelta * 1.2f);

}

void CWeapon::MotionIdle(_float fTimeDelta, _float fPosY)
{
	_float fSpeed = m_pPlayer->GetPlayerState() == IDLE ? 0.002f : 0.009f;
	_float fRange = m_pPlayer->GetPlayerState() == IDLE ? 0.012f : 0.025f;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_iIdle == 1)
	{
		m_pTransformCom->Go_Up(fTimeDelta * fSpeed);

		if (XMVectorGetY(vPos) >= (fPosY + fRange))
			m_iIdle *= -1;
	}
	if (m_iIdle == -1)
	{
		m_pTransformCom->Go_Down(fTimeDelta * fSpeed);

		if (XMVectorGetY(vPos) <= (fPosY - fRange))
			m_iIdle *= -1;
	}
}

void CWeapon::LoadGun()
{
	int iMagazine = m_iBulletMagazine;
	int iBulletRemain = m_iBulletMagazine - m_iBulletCurr;	// ÀåÀü Àü ³²Àº ÃÑ¾Ë

	//if (m_iBulletMagazine > m_iBulletTot)					// if (40¹ß > 29¹ß) -> 29¹ß ÀåÀü		// Àß¸øµÊ
	//	iMagazine = m_iBulletTot;

	m_iBulletCurr = iMagazine;
	m_iBulletTot -= iBulletRemain;		// ³²Àº Åº ÃÑ °³¼ö = ÀåÀü Àü ³²Àº ÃÑ¾Ë
}

void CWeapon::LoadGun(_uint i)
{
	m_iBulletCurr += i;
	m_iBulletTot -= i;		// ³²Àº Åº ÃÑ °³¼ö = ÀåÀü Àü ³²Àº ÃÑ¾Ë
	PLAY(L"wpnBlueEyeReload", false);
}

void CWeapon::CreateBullet()
{
	/* create Bullet */
	_vector playerPos = m_pParentTransform->Get_State(CTransform::STATE_POSITION);

	CPlayer_Bullet::PLAYER_BULLET		PlayerBulletDesc{};

	PlayerBulletDesc.fSpeedPerSec = 70.f;
	PlayerBulletDesc.worldMat = *m_pParentTransform->Get_WorldFloat4x4();
	PlayerBulletDesc.fAttack = m_fAttack;

	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Bullet", L"Prototype_GameObject_Player_Bullet", &PlayerBulletDesc);
}

void CWeapon::CreateBlueBullet()
{
	for(int i = 0; i<3; ++i)
	{
		_float randX = (rand() % 5) / 10.f;
		_float randY = (rand() % 5) / 10.f;

		/* create Bullet */
		_vector playerPos = m_pParentTransform->Get_State(CTransform::STATE_POSITION);


		CPlayer_BlueBullet::PLAYER_BLUEBULLET		PlayerBulletDesc{};

		PlayerBulletDesc.fSpeedPerSec = 60.f;
		PlayerBulletDesc.worldMat = *m_pParentTransform->Get_WorldFloat4x4();
		PlayerBulletDesc.fAttack = m_fAttack / 4.f;
		PlayerBulletDesc.vTargetPos = { randX, randY, 1.f, 1.f};

		m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Bullet", L"Prototype_GameObject_Player_BlueBullet", &PlayerBulletDesc);
	}
}

void CWeapon::ShowWeapon()
{
	m_eState = SHOW;
	m_bUseYN = true;
}

void CWeapon::HideWeapon()
{
	m_eState = CHANGE;
}

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CWeapon::Clone(void* pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
