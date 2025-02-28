#pragma once

#include "VIBuffer.h"

/* 같은 모델을 다수 찍어내야하는 클래스들의 부모를 담당하는 클래스 */
/* 그려지고 있는 각각의 로컬 상태변환을 담당(행렬)하는 정점들을 가지고 있는 정점버퍼하나를 추가적으로 보유한다. */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	typedef struct
	{
		_float2		vScale{};
		_float2		vSpeed{};
		_float2		vLifeTime{}; /* 각 입자의 렌더링시간. */
		_float3		vPivot{}; /* 움직임을 가져가기위한 기준 점?! */
		_float3		vCenter{}; /* 생성위치의 중점. */
		_float3		vRange{};
		_bool		isLoop{}; /*이 이펙트 전체의 재생관련. 루프가 트루라면 각 입자의 재생시간(lifetime)이 끝났을 때, 각 입자가. 다시 재 시작할 수 있도록 하자.  */
		_uint		iNumInstance{};
	}INSTANCE_DESC;

protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;

public:
	virtual void Update_Spread(_float fTimeDelta);
	virtual void Update_Drop(_float fTimeDelta);

protected:
	/* 추가적 정점(행렬)을 가진다. */
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	_uint						m_iNumInstance = {};
	_uint						m_iInstanceVertexStride = {};
	_uint						m_iNumIndexPerInstance = {};
	_float*						m_pSpeeds = { nullptr };
	_float3						m_vPivot = {};
	_bool						m_isLoop = { false };
	_float4*					m_pInstanceVertexPos = { nullptr };

protected:
	/* 0.0f ~ 1.f */
	_float Compute_RandomNumber() {
		return rand() / (_float)RAND_MAX;
	}

	/* fMin ~ fMax */
	_float Compute_RandomNumber(_float fMin, _float fMax) {
		return fMin + Compute_RandomNumber() * (fMax - fMin);
	}


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END