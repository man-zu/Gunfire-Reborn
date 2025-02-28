#include "Bounding_AABB.h"
#include "DebugDraw.h"


#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_AABB::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	if (nullptr == pBoundingDesc)
		return E_FAIL;

	BOUNDING_AABB_DESC* pDesc = static_cast<BOUNDING_AABB_DESC*>(pBoundingDesc);

	m_pBoundingDesc_Original = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pBoundingDesc = new BoundingBox(*m_pBoundingDesc_Original);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]).m128_f32[0];
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]).m128_f32[0];
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]).m128_f32[0];

	m_pBoundingDesc_Original->Transform(*m_pBoundingDesc, TransformMatrix);
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eColliderType, CBounding* pTargetBounding)
{
	m_isCollision = false;

	switch (eColliderType)
	{
	case CCollider::TYPE_AABB:
		m_isCollision = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_AABB*>(pTargetBounding)->Get_Desc()));
		break;
	case CCollider::TYPE_OBB:
		m_isCollision = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_OBB*>(pTargetBounding)->Get_Desc()));
		break;
	case CCollider::TYPE_SPHERE:
		m_isCollision = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_Sphere*>(pTargetBounding)->Get_Desc()));
		break;
	}
	
	return m_isCollision;
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	// 

	DX::Draw(pBatch, *m_pBoundingDesc, false == m_isCollision ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));

	return S_OK;
}
#endif

//_bool CBounding_AABB::Intersect_ToAABB(const CBounding_AABB* pTargetBounding)
//{
//	m_isCollision = false;
//
//	_float3		vSourMin = Compute_Min();
//	_float3		vSourMax = Compute_Max();
//
//	_float3		vDestMin = pTargetBounding->Compute_Min();
//	_float3		vDestMax = pTargetBounding->Compute_Max();
//
//	/* �ʺ������ ��ġ�� �ʾҳ�?! */
//	if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
//		return m_isCollision;
//
//	/* ���̻����� ��ġ�� �ʾҳ�?! */
//	if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
//		return m_isCollision;
//
//	/* ���̻����� ��ġ�� �ʾҳ�?! */
//	if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
//		return m_isCollision;
//
//	m_isCollision = true;
//
//	return m_isCollision;
//}
//
//_float3 CBounding_AABB::Compute_Min() const
//{
//	_float3		vMin = {};
//
//	XMStoreFloat3(&vMin, XMLoadFloat3(&m_pBoundingDesc->Center) - XMLoadFloat3(&m_pBoundingDesc->Extents));
//
//	return vMin;
//}
//
//_float3 CBounding_AABB::Compute_Max() const
//{
//	_float3		vMax = {};
//
//	XMStoreFloat3(&vMax, XMLoadFloat3(&m_pBoundingDesc->Center) + XMLoadFloat3(&m_pBoundingDesc->Extents));
//
//	return vMax;
//}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_AABB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc_Original);
	Safe_Delete(m_pBoundingDesc);
}

