#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "Model.h"


BEGIN(Engine)
class CNavigation;
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster_Arrow final : public CMonster
{
private:
	CMonster_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Arrow(const CMonster_Arrow& rhs);
	virtual ~CMonster_Arrow() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);
	const _uint Get_CurStateIdx() { return m_pModelCom->getCurAnimIdx(); }
	MONSTERSTATE GetCurBoomState() { return m_eMonsterState; }

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
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

	_float4					m_pPosMove = { 0.f, 0.f, 0.f, 1.f };	// 보스 초기 위치 보정

	MONSTERSTATE			m_eMonsterState = { MONSTERSTATE::MONSTER_END };
	_uint					m_iStateCurNum = 0;
	_uint					m_iShotCnt = 0;

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
	void createArrow();
	void CheckColl();

public:
	CCollider* GetColl() { return m_pColliderCom; }
	void Set_State(MONSTERSTATE eState) { m_eMonsterState = eState; m_pModelCom->Get_Finished(); m_iStateCurNum = 0; }

public:
	static CMonster_Arrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END