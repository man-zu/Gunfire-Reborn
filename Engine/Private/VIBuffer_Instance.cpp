#include "..\Public\VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance & rhs)
	: CVIBuffer{ rhs }
	, m_pVBInstance{ rhs.m_pVBInstance }
	, m_iNumInstance { rhs.m_iNumInstance }
	, m_iInstanceVertexStride { rhs.m_iInstanceVertexStride }
	, m_iNumIndexPerInstance { rhs.m_iNumIndexPerInstance }
	, m_pSpeeds { rhs.m_pSpeeds }
	, m_vPivot { rhs.m_vPivot }
	, m_pInstanceVertexPos { rhs.m_pInstanceVertexPos }
	, m_isLoop { rhs.m_isLoop }
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc)
{
	m_iNumInstance = pInstanceDesc->iNumInstance;
	m_vPivot = pInstanceDesc->vPivot;
	m_isLoop = pInstanceDesc->isLoop;
	
	m_pSpeeds = new _float[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		m_pSpeeds[i] = Compute_RandomNumber(pInstanceDesc->vSpeed.x, pInstanceDesc->vSpeed.y);
	}

	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Buffers()
{
	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint					iStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride,
	};

	_uint					iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{
	
	m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instance::Update_Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		/* void*로 뽑아낸 정점 배열의 가장 앞 주소를 1byte포인터형으로 형변화하여 i * m_iInstanceVertexStride를 더할때 마다 정점 하나씩 건너갈 수 있도록 처리 */
		/* _ubyte*		pVertices  == i번째 정점의 가장 앞 주소. */
		_ubyte*		pVertices = static_cast<_ubyte*>(SubResource.pData) + i * m_iInstanceVertexStride;

		_float3*	pVertexPosition = reinterpret_cast<_float3*>(pVertices);

		_vector		vDir = XMLoadFloat3(pVertexPosition) - XMLoadFloat3(&m_vPivot);

		XMStoreFloat3(pVertexPosition, XMLoadFloat3(pVertexPosition) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::Update_Drop(_float fTimeDelta)
{

}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pInstanceVertexPos);
	}

	Safe_Release(m_pVBInstance);
}
