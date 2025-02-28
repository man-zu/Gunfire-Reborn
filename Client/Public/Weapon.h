#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon : public Engine::CPartObject
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
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT InitializeChild(void* pArg);
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual void MotionControl() {};
	virtual void MotionWeaponChange(_float fTimeDelta);
	virtual void MotionIdle(_float fTimeDelta, _float fPosY);
	virtual void LoadGun();
	virtual void LoadGun(_uint i);
	virtual void CreateBullet();
	virtual void CreateBlueBullet();

public:
	virtual void ShowWeapon();
	virtual void HideWeapon();
	WEAPONTYPE GetWeaponType() { return eWeaponType; }
	_uint		GetCurWeaponBulletCur() { return m_iBulletCurr; }
	_uint		GetCurWeaponBulletTot() { return m_iBulletTot; }
	_bool GetShowYN() { return m_bUseYN; }
	void SetUse(_bool _b) { m_bUseYN = _b; }

public:
	CTransform* getTransForm() { return m_pTransformCom; }
	_float		GetCurWeaponAttack() { return m_fAttack; }

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


protected:
	/* COMMON */
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

	/* WEAPON */
	WEAPONSATE				m_eState = { IDLE };
	WEAPONTYPE				eWeaponType = { WEAPON_END };

	_uint					m_iBulletTot = 0;		// ÃÑ Åº °³¼ö
	_uint					m_iBulletCurr = 0;		// ÀåÀüµÇ¾î ÀÖ´Â ÅºÃ¢ Áß ³²Àº Åº °³¼ö
	_uint					m_iBulletMagazine = 0;	// ÇÑÅºÃ¢ Max Åº °³¼ö
	_bool					m_bUseYN = { false };
	_float					m_fTimeAcc = { 0.f };

	_float4					m_pPosMove = { 0.f, 0.f, 0.f, 1.f };	// ¹«±â À§Ä¡ º¸Á¤

	/* Control Weapon State */
	_int					m_iIdle = 1;


};

END