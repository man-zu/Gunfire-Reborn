#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "Monster_BoomPart.h"

BEGIN(Engine)
class CCollider;
class CPartObject;
END

BEGIN(Client)

class CMonster_Boom final : public CMonster
{
private:
	CMonster_Boom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Boom(const CMonster_Boom& rhs);
	virtual ~CMonster_Boom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CPartObject*>			m_Parts;
	CMonster_BoomPart*				m_pBody = { nullptr };
	CNavigation*					m_pNavigationCom = { nullptr };

private:
	HRESULT Ready_PartObjects(void* pArg);
	HRESULT Ready_Components();
	void LandObjectMap();

public:
	CCollider* GetColl() { return m_pBody->GetColl(); }

public:
	static CMonster_Boom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END