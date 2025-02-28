#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Weapon.h"
#include "Boss_65_Body.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CPartObject;
END

BEGIN(Client)

class CBoss : public CGameObject
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_float fAttack = 0;
	}BOSSSKILL;

protected:
	CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss(const CBoss& rhs);
	virtual ~CBoss() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CPartObject*>		m_Parts;
	class CBoss_65_Body*		m_pBody = { nullptr };

	CNavigation*				m_pNavigationCom = { nullptr };

	_bool						m_bChargeShield = { false };
	_bool						m_bChargeSeconeShield = { false };

	_float						m_fJumpTime = 0.f;
	_float						m_fGravity = 9.8f;
	_float						m_fJumpPower = 1.0f;
	_bool						m_bJump = false;
	_bool						m_bJumpUp = false;		// 돌산 위로 올라가는 점프
	_bool						m_bFall = false;
	_bool						m_JumpFromRock = false;
	_bool						m_Once = false;
	_float4						m_pPosTarget = { 0.f, 0.f, 0.f, 1.f };		// (공격패턴) 공격할 위치

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void JumpDown(_float fTimeDelta);
	void JumpUp(_float fTimeDelta);

private:
	void CreateBoom();

public:
	/* MovePattern */
	void BossJumpFromRock(_float fTimeDelta);
	void BossJumpAttack(_float fTimeDelta, _float4 vTargetPos);
	/* Effect */
	void CreatePunchFloor();
	void CreateThrowStone();
	void CreateJumpStone();
	void CreateTargetRing(_float4 PosTarget);
	void CreateShot();

	
	_bool BossHealling(_float fTimeDelta);

	_vector GetBossPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	BOSS_PTRN GetBossCurPtrnState() { return m_pBody->GetCurBossState(); }
	void SetCurBossState(BOSS_PTRN eState) { m_pBody->SetCurBossState(eState); }
	class CBoss_65_Body* GetMyBodyPart() { return  m_pBody; }
	void SetJumpUp() { m_bJumpUp = true; }
	void SetJumpFromRock() { m_JumpFromRock = true; m_Once = false; }
	//CCollider** GetColl() { return m_pColliderCom; }

public:
	static CBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END