#include "Bounding_OBB.h"

#include "DebugDraw.h"

#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_OBB::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	if (nullptr == pBoundingDesc)
		return E_FAIL;

	BOUNDING_OBB_DESC* pDesc = static_cast<BOUNDING_OBB_DESC*>(pBoundingDesc);

	_float4			vRotation = {};
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pBoundingDesc_Original = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pBoundingDesc = new BoundingOrientedBox(*m_pBoundingDesc_Original);

	return S_OK;
}
void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
	m_pBoundingDesc_Original->Transform(*m_pBoundingDesc, WorldMatrix);
}
_bool CBounding_OBB::Intersect(CCollider::TYPE eColliderType, CBounding* pTargetBounding)
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
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingDesc, false == m_isCollision ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));

	return S_OK;
}
#endif

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_OBB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc_Original);
	Safe_Delete(m_pBoundingDesc);
}
