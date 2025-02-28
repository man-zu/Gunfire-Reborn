#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CPlayer_SR : public CWeapon
{
protected:
	CPlayer_SR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_SR(const CPlayer_SR& rhs);
	virtual ~CPlayer_SR() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	//virtual void MotionControl() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer_SR* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END