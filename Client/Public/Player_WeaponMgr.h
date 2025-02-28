#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer_WeaponMgr : public Engine::CPartObject
{
public:
	enum WEAPONSATE { IDLE, LOAD1, LOAD2, LOAD3, LOAD4, SHOT1, SHOT2, CHANGE, HIDE, SKILL, SHOW };
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4*	pBoneMatrix = { nullptr };
		class CPlayer*		m_pPlayer = { nullptr };
		WEAPONSATE			eWeaponState = { IDLE };
		WEAPONTYPE			eWeaponType = { WEAPON_END };
	}WEAPON_DESC;

protected:
	CPlayer_WeaponMgr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_WeaponMgr(const CPlayer_WeaponMgr& rhs);
	virtual ~CPlayer_WeaponMgr() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();

private:

public:
	CTransform* getTransForm() { return m_pTransformCom; }
	WEAPONTYPE	GetWeaponType() { return m_MyCurWeapon->GetWeaponType(); }
	_uint		GetCurWeaponBulletCur() { return m_MyCurWeapon->GetCurWeaponBulletCur(); }
	_uint		GetCurWeaponBulletTot() { return m_MyCurWeapon->GetCurWeaponBulletTot(); }
	_float		GetCurWeaponAttack() { return m_fAttack; }

public:
	static CPlayer_WeaponMgr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


protected:
	/* COMMON */
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

	/* WEAPON */
	WEAPONTYPE				eWeaponType = { WEAPON_END };


private:
	/* Control Weapon Inven */
	vector<CWeapon*>			m_MyWeapon = {};
	CWeapon*					m_MyCurWeapon = {};
	CWeapon*					m_MyNextWeapon = {};

};

END