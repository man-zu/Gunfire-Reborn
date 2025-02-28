#pragma once

#include "GameObject.h"

/* 클라에서 개발자가 정의할 카메라들에게 공통적으로 필요한 기능이 필요할 수 있겠다. */
/* ViewMatrix 기초 정보들, ProjMatrix 기초 정보들. */
/* 카메라의 움직임을 트랜스폼에 저장했고(월드행렬이 셋팅됐고) -> 역행을 구해서 뷰스페이스 변환행렬로서 장치에 바인딩하는 작업을 수행했다. SetTransform(D3DTS_VIEW, 뷰행렬) */
/* 투영행려을 추후 객체들을 그릴 때 이용할 수 있도록 장치에 바인딩해주는 작업. SetTransform(D3DTS_PROJECTION, 투영행렬) */
/* SetTransform이 없기 때문에 PipeLine에게 뷰, 투영을 저장시킨다. */

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_float4			vEye = {};
		_float4			vAt = {};
		_float			fFovy = {};
		_float			fAspect = {};
		_float			fNear = {};
		_float			fFar = {};
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float					m_fFovy = {};
	_float					m_fAspect = {};
	_float					m_fNear = {};
	_float					m_fFar = {};

protected:
	_bool		m_bMouseSensorUse = { true };
	_bool		m_bShowYN = { false };

public:
	void SetMouseSensorUse(_bool _b) { m_bMouseSensorUse = _b; }
	void SetShowYN(_bool _b) { m_bShowYN = _b; }

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END