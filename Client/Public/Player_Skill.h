#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CPlayer_Skill : public CWeapon
{
protected:
	CPlayer_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Skill(const CPlayer_Skill& rhs);
	virtual ~CPlayer_Skill() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	class CPlayer* m_pPlayer = nullptr;

public:
	static CPlayer_Skill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END