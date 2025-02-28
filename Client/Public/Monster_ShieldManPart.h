#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Model.h"
#include "PartObject.h"


BEGIN(Engine)
class CNavigation;
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster_ShieldManPart final : public CPartObject
{
private:
	CMonster_ShieldManPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_ShieldManPart(const CMonster_ShieldManPart& rhs);
	virtual ~CMonster_ShieldManPart() = default;

public:
	const _uint Get_CurStateIdx() { return m_pModelCom->getCurAnimIdx(); }
	MONSTERSTATE GetCurState() { return m_eMonsterState; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CNavigation*			m_pNavigationCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CCollider*				m_pCollider_Shield = { nullptr };
	CCollider*				m_pCollider_Weapon = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

	_float4					m_pPosMove = { 0.f, 0.f, 0.f, 1.f };	// 몬스터 초기 위치 보정

	MONSTERSTATE			m_eMonsterState = { MONSTERSTATE::MONSTER_END };
	_uint					m_iStateCurNum = 0;
	_bool					m_bRim{ false };

#ifdef _DEBUG
	_uint m_tmp = 0;
#endif

private:
	HRESULT Ready_Components();
	HRESULT Ready_Navi(void* pArg);
	HRESULT Bind_ShaderResources();

private:
	void motion_idle();
	void motion_moveToPlayer(_float fTimeDelta);
	void motion_moveToInitPos(_float fTimeDelta);
	void motion_attack();
	void motion_hit();
	void motion_reset();
	void CheckColl();

public:
	CCollider* GetCollBody() { return m_pColliderCom; }
	CCollider* GetCollShield() { return m_pCollider_Shield; }
	void Set_State(MONSTERSTATE eState) { m_eMonsterState = eState; m_pModelCom->Get_Finished(); m_iStateCurNum = 0; }
	CNavigation* GetNavigation() { return m_pNavigationCom; }

	void SetDissolve() { m_bDissolve = true; }
	void SetDelayTime(_float fTimeDelta) { m_fDelayTime += fTimeDelta; }

public:
	static CMonster_ShieldManPart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END