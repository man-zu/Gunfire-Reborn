#include "Bounding_Sphere.h"
#include "DebugDraw.h"

#include "Bounding_OBB.h"
#include "Bounding_AABB.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_Sphere::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	if (nullptr == pBoundingDesc)
		return E_FAIL;

	BOUNDING_SPHERE_DESC* pDesc = static_cast<BOUNDING_SPHERE_DESC*>(pBoundingDesc);

	m_pBoundingDesc_Original = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pBoundingDesc = new BoundingSphere(*m_pBoundingDesc_Original);

	return S_OK;
}
void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{
	m_pBoundingDesc_Original->Transform(*m_pBoundingDesc, WorldMatrix);
}
_bool CBounding_Sphere::Intersect(CCollider::TYPE eColliderType, CBounding* pTargetBounding)
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
HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingDesc, false == m_isCollision ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));

	return S_OK;
}
#endif
CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc_Original);
	Safe_Delete(m_pBoundingDesc);
}
