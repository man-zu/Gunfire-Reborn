#include "Collider.h"
#include "GameInstance.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_eType{ rhs.m_eType }
	, m_pBounding{ rhs.m_pBounding }
#ifdef _DEBUG
	, m_pBatch{ rhs.m_pBatch }
	, m_pEffect{ rhs.m_pEffect }
	, m_pInputLayout{ rhs.m_pInputLayout }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	/* 이 콜라이더를 그리기위한 준비과정을 수행할거야 .*/
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, static_cast<CBounding::BOUNDING_DESC*>(pArg));
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, static_cast<CBounding::BOUNDING_DESC*>(pArg));
		break;
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, static_cast<CBounding::BOUNDING_DESC*>(pArg));
		break;
	}

	return S_OK;
}
void CCollider::Update(_fmatrix WorldMatrix)
{
	/* CBounding 크랠스는 aabb, obb, sphere의 부모 타입이기때문에. */
	/* 실제 할당된 객체의 Update를 호출하며 월드행렬을 전달할 수 있겠다. */
	m_pBounding->Update(WorldMatrix);
}

_bool CCollider::Intersect(CCollider* pTargetCollider)
{
	/* CBounding 크랠스는 aabb, obb, sphere의 부모 타입이기때문에. */
	/* 실제 할당된 객체의 Intersect를 호출하며 충돌비교한다. */
	return m_pBounding->Intersect(pTargetCollider->m_eType, pTargetCollider->m_pBounding);
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG


CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif // DEBUG
	Safe_Release(m_pBounding);
}
