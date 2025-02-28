#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CPlayer_Pistol : public CWeapon
{
protected:
	CPlayer_Pistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Pistol(const CPlayer_Pistol& rhs);
	virtual ~CPlayer_Pistol() = default;

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

public:
	static CPlayer_Pistol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END