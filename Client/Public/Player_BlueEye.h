#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CPlayer_BlueEye final : public CWeapon
{
private:
	CPlayer_BlueEye(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_BlueEye(const CPlayer_BlueEye& rhs);
	virtual ~CPlayer_BlueEye() = default;

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
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_uint		m_iLoadBulletCnt = { 0 };
	_uint		m_iLoadBulletMotionCnt = { 0 };


public:
	static CPlayer_BlueEye* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END