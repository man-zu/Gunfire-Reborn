#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Player_WeaponMgr.h"
#include "Camera_Player.h"
#include "Weapon.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CPartObject;
END

BEGIN(Client)

class CPlayer : public CGameObject
{
public:
	enum PLAYERSTATE { STOP, MOVE, DAMAGED, DIE, GODMODE };
	enum PLAYERSKILL { DASH, E, Q, P_SKILL_NONE };

	typedef struct TagPlayerDesc : public CTransform::TRANSFORM_DESC
	{
		LEVEL eLevel{ LEVEL_END };
	}PLAYER_DESC;

protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	vector<CPartObject*>		m_Parts;
	CPartObject*				m_pBody = { nullptr };
	CNavigation*				m_pNavigationCom = { nullptr };
	CCollider*					m_pColliderCom = { nullptr };
	class CPlayer_WeaponMgr*	m_pWeaponMgr = { nullptr };
	class CCamera_Player*		m_pCam = { nullptr };

	_float						m_fJumpTime = 0.f;
	_float						m_fGravity = 9.8f;
	_float						m_fJumpPower = 1.5f;
	_bool						m_bJump = false;
	_bool						m_bKnockBack = false;

	_bool						m_bMoveAuto = false;

	_float						m_fGodTime = 0.f;
	_float						m_fDashSpeed = 1.f;
	PLAYERSTATE					m_tPlayerState = { STOP };
	PLAYERSKILL					m_tPlayerSkill = { P_SKILL_NONE };


private:
	_int						m_iKillCnt = 0;


protected:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

	void Jump(_float fTimeDelta);
	void KnockBack(_float fTimeDelta);

private:
	void KeyInput(float fTimeDelta);

public:
	void		DoJump(_bool _b) { m_bJump = _b; };
	void		DoKnockBack(_bool _b) { m_bKnockBack = _b; };

	void		SetMoveSelf() { m_bMoveAuto = false; }
	void		SetAlive() { m_iHp = m_iMaxHp; m_fShield = m_fMaxShield; }
	void		SetPlayerPos(_vector vSetPos) { m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSetPos); }
	void		SetPlayerState(PLAYERSTATE _state) { m_tPlayerState = _state; }
	void		SetDamaged(_float iTargetAtt);

	_uint		GetKillCnt() { return m_iKillCnt; };
	_uint		GetCoinCnt() { return g_bCoin; };
	_uint		GetCurWeaponBulletCur() { return m_pWeaponMgr->GetCurWeaponBulletCur(); }
	_uint		GetCurWeaponBulletTot() { return m_pWeaponMgr->GetCurWeaponBulletTot(); }
	_vector		GetPlayerPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	PLAYERSTATE GetPlayerState() { return m_tPlayerState; }
	WEAPONTYPE	GetWeaponType() { return m_pWeaponMgr->GetWeaponType(); }
	CCollider*	GetColl() { return m_pColliderCom; }
	CCollider*	GetPlayerColl() { return m_pColliderCom; }
	CTransform* GetPlayerTransform() { return m_pTransformCom; }
	CCamera_Player* GetPlayerCam() { return m_pCam; }

	void		AddKillCnt() { m_iKillCnt++; };
	void		GoStraight(_float fTimeDelta) { m_bMoveAuto = true;  m_pTransformCom->Go_Straight(fTimeDelta * m_fDashSpeed, m_pNavigationCom); }
	void		MoveStop(_bool _b) { m_pCam->MoveStop(_b); };
	void		CoinAdd(int coin) { g_bCoin += coin; };
	
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END