#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = {};
	}CAMERA_FREE_DESC;

private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float		m_fMouseSensor = {};
	_bool		m_bBossScene = false;
	_vector		m_pPosStart = { };	// 씬 카메라 시작위치

private:
	void		BossScene(_float fTimeDelta);

public:
	void		BossSceneYN(_bool _b, _float4x4 curPosMat) { m_bShowYN = _b; m_bBossScene = _b; m_pTransformCom->Set_WorldMat(curPosMat); }
	_bool		IsBossScene() { return m_bBossScene; }

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END