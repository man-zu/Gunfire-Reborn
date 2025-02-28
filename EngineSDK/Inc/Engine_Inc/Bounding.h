#pragma once

#include "Collider.h"

/* Sphere, AABB, oBB의 부모가 되는 클래스다.  */

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct
	{
		_float3		vCenter;
	}BOUNDING_DESC;
protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix) = 0;
	virtual _bool Intersect(CCollider::TYPE eColliderType, CBounding* pTargetBounding) = 0;


#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);
#endif

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_bool						m_isCollision = { false };

public:
	virtual void Free() override;
};

END