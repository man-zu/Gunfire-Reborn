#include "..\Public\VIBuffer_Point_Instance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instance { pDevice, pContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance & rhs)
	: CVIBuffer_Instance { rhs }
	, m_pInstanceVertices { rhs.m_pInstanceVertices }
	
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc)
{
	if (FAILED(__super::Initialize_Prototype(pInstanceDesc)))
		return E_FAIL;

	/* ���ϳ��׸��µ� �ε��� �� �� �ʿ���? */
	m_iNumIndexPerInstance = 1;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_iIndexStride = 2;
	m_iNumIndices = m_iNumIndexPerInstance * m_iNumInstance;
	m_iNumVertexBuffers = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	/* �����ϰ����ϴ� ������ �Ӽ��� �����ϳ�. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* �����ϰ����ϴ� ������ �ʱⰪ�� �����Ѵ�. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXPOINT*		pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = XMFLOAT3(0.f, 0.0f, 0.f);
	pVertices[0].vPSize = XMFLOAT2(1.f, 1.f);

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	_ushort*		pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);
	
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	

#pragma endregion

	m_pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

	if(true == pInstanceDesc->isLoop)
	{
		/* �ٽ� ó����ġ�� ���ư� �� �ֵ��� �ʱ���ġ�� �����ϱ����� ������ �Ҵ�����. */
		m_pInstanceVertexPos = new _float4[m_iNumInstance];
		ZeroMemory(m_pInstanceVertexPos, sizeof(_float4) * m_iNumInstance);
	}

	
	/* �ν��Ͻ� �������ۿ� ���ִ� �� ����(���)�� ���¸� �������� �����Ѵ�.*/
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float		fScale = Compute_RandomNumber(pInstanceDesc->vScale.x, pInstanceDesc->vScale.y);
		_float		fLifeTime = Compute_RandomNumber(pInstanceDesc->vLifeTime.x, pInstanceDesc->vLifeTime.y);

		m_pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		m_pInstanceVertices[i].vTranslation = _float4(
			Compute_RandomNumber(pInstanceDesc->vCenter.x - pInstanceDesc->vRange.x * 0.5f, pInstanceDesc->vCenter.x + pInstanceDesc->vRange.x * 0.5f),
			Compute_RandomNumber(pInstanceDesc->vCenter.y - pInstanceDesc->vRange.y * 0.5f, pInstanceDesc->vCenter.y + pInstanceDesc->vRange.y * 0.5f),
			Compute_RandomNumber(pInstanceDesc->vCenter.z - pInstanceDesc->vRange.z * 0.5f, pInstanceDesc->vCenter.z + pInstanceDesc->vRange.z * 0.5f),
			1.f);

		if (true == pInstanceDesc->isLoop)
			m_pInstanceVertexPos[i] = m_pInstanceVertices[i].vTranslation;

		m_pInstanceVertices[i].vLifeTime.x = fLifeTime;
	}





	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void * pArg)
{
#pragma region INSTANCE_BUFFER

	m_iInstanceVertexStride = sizeof(VTXMATRIX);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iInstanceVertexStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pInstanceVertices;

	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;

#pragma endregion


	return S_OK;
}

void CVIBuffer_Point_Instance::Update_Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{		
		VTXMATRIX*		pVertices = static_cast<VTXMATRIX*>(SubResource.pData) + i;		

		_vector		vDir = XMLoadFloat4(&pVertices->vTranslation) - XMVectorSetW(XMLoadFloat3(&m_vPivot), 1.f);

		XMStoreFloat4(&pVertices->vTranslation, XMLoadFloat4(&pVertices->vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);

		pVertices->vLifeTime.y += fTimeDelta;

		if(true == m_isLoop && pVertices->vLifeTime.y >= pVertices->vLifeTime.x)
		{
			pVertices->vTranslation = m_pInstanceVertexPos[i];
			pVertices->vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Update_Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		VTXMATRIX*		pVertices = static_cast<VTXMATRIX*>(SubResource.pData) + i;

		_vector		vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

		XMStoreFloat4(&pVertices->vTranslation, XMLoadFloat4(&pVertices->vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);

		pVertices->vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices->vLifeTime.y >= pVertices->vLifeTime.x)
		{
			pVertices->vTranslation = m_pInstanceVertexPos[i];
			pVertices->vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Point_Instance * CVIBuffer_Point_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const INSTANCE_DESC* pInstanceDesc)
{
	CVIBuffer_Point_Instance*		pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pInstanceDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CVIBuffer_Point_Instance::Clone(void * pArg)
{
	CVIBuffer_Point_Instance*		pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertices);
	}
	
}
