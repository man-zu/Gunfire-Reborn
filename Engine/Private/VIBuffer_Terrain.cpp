#include "VIBuffer_Terrain.h"
#include "Picking.h"
#include "GameInstance.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer( rhs )
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath, _uint iX, _uint iZ)
{
	_ulong		dwByte = { 0 };

	HANDLE		hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER			fh{};
	BITMAPINFOHEADER			ih{};
	_uint* pPixel = { nullptr };

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	m_iNumVerticesX = iX;
	m_iNumVerticesZ = iZ;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	pPixel = new _uint[m_iNumVertices];
	ZeroMemory(pPixel, sizeof(_uint) * m_iNumVertices);

	ReadFile(hFile, pPixel, sizeof(_uint) * m_iNumVertices, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iVertexStride = sizeof(VTXNORTEX);
	m_iIndexStride = 4;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVertexPositions[iIndex] = _float3((_float)j, 0.f, (_float)i);
			//pVertices[iIndex].vPosition = m_pVertexPositions[iIndex] = _float3((_float)j, (pPixel[iIndex] & 0x000000ff) / 10.f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), (_float)i / (m_iNumVerticesZ - 1.f));
		}
	}
	Safe_Delete_Array(pPixel);

#pragma endregion


#pragma region INDEX_BUFFER	

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = _uint(i * m_iNumVerticesX + j);

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSour, vDest, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);

			vNormal = XMVector3Cross(vSour, vDest);

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);

			vNormal = XMVector3Cross(vSour, vDest);

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);

		}
	}

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}


#pragma endregion

	/* 생성하고자하는 버퍼의 속성을 설정하낟. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 생성하고자하는 버퍼의 초기값을 설정한다. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);


	return S_OK;
}


HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}


_bool CVIBuffer_Terrain::Picking(CTransform* pTransform, _float3* pOut)
{
	CPicking* pPicking = m_pGameInstance->Get_Picking();
	Safe_AddRef(pPicking);

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, pTransform->Get_WorldMatrix());	// 터레인의 transform 월드행렬을 가져옴

	_float3		vRayDir, vRayPos;		// (로컬)

	pPicking->Compute_LocalRayInfo(&vRayDir, &vRayPos, &WorldMatrix);

	XMVector3Normalize(XMLoadFloat3(&vRayDir));

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_float		fU, fV, fDist;
			// 
			_bool bCheckPicking = pPicking->IntersectTri(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir), 
				XMLoadFloat3(&m_pVertexPositions[iIndices[0]]), XMLoadFloat3(&m_pVertexPositions[iIndices[1]]), XMLoadFloat3(&m_pVertexPositions[iIndices[2]]),
				&fDist, &fU, &fV);
			
			if (true == bCheckPicking)
			{
				XMVECTOR vRayDD = XMVectorMultiply(XMLoadFloat3(&vRayDir), { fDist, fDist , fDist });
				XMVECTOR vPickPos = XMVectorAdd(XMLoadFloat3(&vRayPos), vRayDD);

				XMVector3TransformCoord(vPickPos, XMLoadFloat4x4(&WorldMatrix)); // 피킹 된 점의 좌표를 월드를 곱해서 월드로 옮겨줌
				XMStoreFloat3(pOut, vPickPos);

				Safe_Release(pPicking);
				return true;
			}

			bCheckPicking = pPicking->IntersectTri(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir), 
				XMLoadFloat3(&m_pVertexPositions[iIndices[0]]), XMLoadFloat3(&m_pVertexPositions[iIndices[2]]), XMLoadFloat3(&m_pVertexPositions[iIndices[3]]),
				&fDist, &fU, &fV);
			
			if (true == bCheckPicking)
			{
				XMVECTOR vRayDD = XMVectorMultiply(XMLoadFloat3(&vRayDir), {fDist, fDist , fDist});
				XMVECTOR vPickPos = XMVectorAdd(XMLoadFloat3(&vRayPos), vRayDD);

				XMVector3TransformCoord(vPickPos, XMLoadFloat4x4(&WorldMatrix));
				XMStoreFloat3(pOut, vPickPos);

				Safe_Release(pPicking);
				return true;
			}
		}
	}

	Safe_Release(pPicking);
	return false;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath, _uint iX, _uint iZ)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath, iX, iZ)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
