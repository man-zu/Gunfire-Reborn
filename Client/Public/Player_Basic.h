#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CPlayer_Basic : public CWeapon
{
protected:
	CPlayer_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Basic(const CPlayer_Basic& rhs);
	virtual ~CPlayer_Basic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual void MotionControl() override;

private:

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer_Basic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END