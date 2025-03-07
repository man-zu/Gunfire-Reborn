#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
	}BOUNDING_AABB_DESC;
private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	const BoundingBox* Get_Desc() const {
		return m_pBoundingDesc;
	}

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eColliderType, CBounding* pTargetBounding) override;

#ifdef _DEBUG
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif
private:
	BoundingBox* m_pBoundingDesc_Original = { nullptr };
	BoundingBox* m_pBoundingDesc = { nullptr };

private:
	//_bool Intersect_ToAABB(const CBounding_AABB* pTargetBounding);
	//_float3 Compute_Min() const;
	//_float3 Compute_Max() const;

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END