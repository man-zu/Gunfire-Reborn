#pragma once

#include "GameObject.h"

/* Ŭ�󿡼� �����ڰ� ������ ī�޶�鿡�� ���������� �ʿ��� ����� �ʿ��� �� �ְڴ�. */
/* ViewMatrix ���� ������, ProjMatrix ���� ������. */
/* ī�޶��� �������� Ʈ�������� �����߰�(��������� ���õư�) -> ������ ���ؼ� �佺���̽� ��ȯ��ķμ� ��ġ�� ���ε��ϴ� �۾��� �����ߴ�. SetTransform(D3DTS_VIEW, �����) */
/* ��������� ���� ��ü���� �׸� �� �̿��� �� �ֵ��� ��ġ�� ���ε����ִ� �۾�. SetTransform(D3DTS_PROJECTION, �������) */
/* SetTransform�� ���� ������ PipeLine���� ��, ������ �����Ų��. */

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