#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "Monster_ShieldManPart.h"

BEGIN(Engine)
class CCollider;
class CPartObject;
END

BEGIN(Client)

class CMonster_ShieldMan final : public CMonster
{
private:
	CMonster_ShieldMan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_ShieldMan(const CMonster_ShieldMan& rhs);
	virtual ~CMonster_ShieldMan() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CPartObject*>			m_Parts;
	CMonster_ShieldManPart*			m_pBody = { nullptr };
	_float4							m_pPosMove = { 0.f, 0.f, 0.f, 1.f };	// 몬스터 초기 위치 보정

private:
	HRESULT Ready_PartObjects(void* pArg);
	HRESULT Ready_Components();
	void LandObjectMap();

public:
	CCollider* GetCollBody() { return m_pBody->GetCollBody(); }
	CCollider* GetCollShield() { return m_pBody->GetCollShield(); }
	void Set_State(MONSTERSTATE eState) { m_pBody->Set_State(eState); }

public:
	static CMonster_ShieldMan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END