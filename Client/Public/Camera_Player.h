#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CTransform;
END

BEGIN(Client)

class CCamera_Player final : public Engine::CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float4			vEye = {};
		_float4			vAt = {};
		_float			fFovy = {};
		_float			fAspect = {};
		_float			fNear = {};
		_float			fFar = {};
		_float			fMouseSensor = {};
		CNavigation*	m_pPlayerNavigationCom = { nullptr };
		class CPlayer*	m_pPlayer = { nullptr };
	}PLAYER_CAMERA_DESC;

private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CCollider*				m_pColliderCom = { nullptr };

	PLAYER_CAMERA_DESC		m_tPlayerCam = {};
	_bool					m_bShowYN = { true };
	_bool					m_bMoveStop = { false };

	_float4					m_pPosMove = { 0.f, 0.f, 0.f, 1.f };	// 카메라 위치 보정

	class CPlayer*			m_pPlayer = { nullptr };

private:
	HRESULT Ready_Components();

public:
	void SetShowYN(_bool _b) { m_bShowYN = _b; }
	void MoveStop(_bool _b) { m_bMoveStop = _b; }
	_float4x4 CurPosMatrix() { return *m_pParentTransform->Get_WorldFloat4x4(); }

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END