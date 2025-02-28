#include "Player_WeaponMgr.h"

#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Player_Bullet.h"
#include "Player_Basic.h"
#include "Player_BlueEye.h"
#include "Player_SR.h"
#include "Player_Pistol.h"
#include "Player_Skill.h"

CPlayer_WeaponMgr::CPlayer_WeaponMgr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CPlayer_WeaponMgr::CPlayer_WeaponMgr(const CPlayer_WeaponMgr& rhs)
	: CPartObject(rhs)
{
}

HRESULT CPlayer_WeaponMgr::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pPlayer = pDesc->m_pPlayer;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_WeaponMgr::PriorityTick(_float fTimeDelta)
{
	if (m_MyCurWeapon != nullptr)
	{
		if (false == m_MyCurWeapon->GetShowYN())
		{
			m_MyNextWeapon->ShowWeapon();
			m_MyCurWeapon = m_MyNextWeapon;
			eWeaponType = m_MyCurWeapon->GetWeaponType();
		}
	}

	if (KEY_PUSH(DIK_1))
	{
		if (m_MyCurWeapon != nullptr)
			m_MyCurWeapon->HideWeapon();

		m_MyNextWeapon = m_MyWeapon[0];
	}
	else if (KEY_PUSH(DIK_2))
	{
		if (m_MyCurWeapon != nullptr)
			m_MyCurWeapon->HideWeapon();

		m_MyNextWeapon = m_MyWeapon[1];
	}
	else if (KEY_PUSH(DIK_3))
	{
		if (m_MyCurWeapon != nullptr)
			m_MyCurWeapon->HideWeapon();

		m_MyNextWeapon = m_MyWeapon[2];
	}
	else if (KEY_PUSH(DIK_Q) || KEY_PUSH(DIK_E))
	{
		m_MyCurWeapon->HideWeapon();
		m_MyNextWeapon = m_MyCurWeapon;
	}

	if (m_MyCurWeapon != nullptr)
		m_MyCurWeapon->PriorityTick(fTimeDelta);

}

void CPlayer_WeaponMgr::Tick(_float fTimeDelta)
{
	if (m_MyCurWeapon != nullptr)
		m_MyCurWeapon->Tick(fTimeDelta);
}

void CPlayer_WeaponMgr::LateTick(_float fTimeDelta)
{
	if (m_MyCurWeapon != nullptr)
		m_MyCurWeapon->LateTick(fTimeDelta);
}

HRESULT CPlayer_WeaponMgr::Render()
{
	return S_OK;
}

HRESULT CPlayer_WeaponMgr::Ready_Components()
{
	CPlayer_WeaponMgr::WEAPON_DESC		WeaponDesc{};
	//WeaponDesc.pBoneMatrix = dynamic_cast<CPlayer_Body*>(pBody)->Get_BoneMatrixPtr("SWORD");
	/* 추가된 파트오브젝트가 부모를 기준으로 회전하거나 이동할 수 있다. */

	WeaponDesc.fSpeedPerSec = 5.f;
	WeaponDesc.pParentTransform = m_pParentTransform;
	WeaponDesc.eWeaponType = WEAPONTYPE::SKILL;
	WeaponDesc.m_pPlayer = m_pPlayer;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Gun", L"Prototype_GameObject_Player_Skill", &WeaponDesc)))
		return E_FAIL;

	for (int i = WEAPONTYPE::BASIC; i < WEAPONTYPE::WEAPON_END; ++i)
	{
		WeaponDesc.eWeaponType = (WEAPONTYPE)i;
		CWeapon* pWeapon = nullptr;

		switch (i)
		{
			case 1:
				pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Player_Basic", &WeaponDesc));
				break;
			case 2:
				pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Player_BlueEye", &WeaponDesc));
				break;
			case 3:
				pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Player_SR", &WeaponDesc));
				break;
			case 4:
				pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Player_Pistol", &WeaponDesc));
				break;
		}

		if (nullptr == pWeapon)
			return E_FAIL;
		m_MyWeapon.push_back(pWeapon);
	}

	m_MyCurWeapon = m_MyWeapon[0];
	m_MyCurWeapon->SetUse(true);
	eWeaponType = m_MyCurWeapon->GetWeaponType();

	return S_OK;
}

CPlayer_WeaponMgr* CPlayer_WeaponMgr::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_WeaponMgr* pInstance = new CPlayer_WeaponMgr(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_WeaponMgr"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CPlayer_WeaponMgr::Clone(void* pArg)
{
	CPlayer_WeaponMgr* pInstance = new CPlayer_WeaponMgr(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_WeaponMgr"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_WeaponMgr::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

	if (m_MyWeapon.size() <= 0)
		return;

	CPlayer_Basic* tmp1 = dynamic_cast<CPlayer_Basic*>(m_MyWeapon[0]);
	Safe_Release(tmp1);
	CPlayer_BlueEye* tmp2 = dynamic_cast<CPlayer_BlueEye*>(m_MyWeapon[1]);
	Safe_Release(tmp2);
	CPlayer_SR* tmp3 = dynamic_cast<CPlayer_SR*>(m_MyWeapon[2]);
	Safe_Release(tmp3);
	CPlayer_Pistol* tmp4 = dynamic_cast<CPlayer_Pistol*>(m_MyWeapon[3]);
	Safe_Release(tmp4);
}
