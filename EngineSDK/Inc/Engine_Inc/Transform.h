#pragma once

#include "Component.h"

/* XMFLOAT2, XMFLOAT3, XMFLOAT4 == XMVECTOR */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct
	{
		_int		iCurrentCellIndex = 0;
		_float		fSpeedPerSec = {};
		_float		fRotationPerSec = {};
		_float		fTurn = 0.f;
		_float4		vInitPos = {0.f, 0.f, 0.f, 1.f};
		_float4x4	worldMat = {};
	}TRANSFORM_DESC;

public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	_float3 Get_Scaled();

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	const _float4x4* Get_WorldFloat4x4() const {
		return &m_WorldMatrix;
	}

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], vState);
	}
	void Set_Scale(const _float3& vScale);

	void Set_WorldMat(_float4x4 _mat) {
		m_WorldMatrix = _mat;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override; 

public:
	void Go_InitXYZ(_float fX, _float fY, _float fZ);	// Init 초기 위치 보정
	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Left(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Down(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation(_float fX, _float fY, _float fZ);
	void Turn(_fvector vAxis, _float fTimeDelta);
	_bool TurnTarget(_fvector vAxis, _float fTimeDelta, CTransform* pTarget);
	_bool TurnTarget(_fvector vAxis, _float fTimeDelta, _float4x4 mat);
	_bool TurnTargetYN(_fvector vAxis, _float fTimeDelta, CTransform* pTarget);
	void LookAt(_fvector vAt);
	void LookAtForLandObject(_fvector vTargetPos);
	void Go_Target(_float fTimeDelta, _fvector vTargetPos, class CNavigation* pNavigation = nullptr);
	void Go_TargetDir(_float fTimeDelta, _fvector vTargetPos);
	_bool Go_DistanceCheck(const _fvector vTargetPos, _float fMargin);
	_bool Go_SavedWorldMatrix(_float fTimeDelta);
	void MoveSlidingV(_float fTimeDelta, _fvector vMoveVector, _fvector vContactNormal, CNavigation* pNavigation);
	void Turn_Target(_float fTimeDelta, _fvector vTargetPos);
	void Set_Billboard();

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	void SaveStatusWorld() { XMStoreFloat4x4(&m_SaveWorldMatrix, XMLoadFloat4x4(&m_WorldMatrix)); }
	void LoadStatusWorld() { XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(&m_SaveWorldMatrix)); }

private:
	_float4x4		m_WorldMatrix = {};
	_float4x4		m_SaveWorldMatrix = {};
	_float			m_fSpeedPerSec = { 0.f };
	_float			m_fRotationPerSec = { 0.f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END