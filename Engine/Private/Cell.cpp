#include "Cell.h"
#include "VIBuffer_Cell.h"
#include "VIBuffer_Line.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex, POINT ePoint)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * ePoint);

	m_iIndex = iIndex;

#ifdef _DEBUG
	if(ePoint == POINT_END)
	{
		m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
		if (nullptr == m_pVIBuffer)
			return E_FAIL;
	}
	else
	{
		m_pVIBufferLine = CVIBuffer_Line::Create(m_pDevice, m_pContext, m_vPoints);
		if (nullptr == m_pVIBufferLine)
			return E_FAIL;
	}
#endif

	return S_OK;
}

_bool CCell::isIn(_fvector vLocalPos, _int* pNeighborIndex, _float3* pOutNormal)
{
	_vector		vDir, vNormal;

	for (size_t i = 0; i < LINE_END; i++)
	{
		vDir = vLocalPos - XMLoadFloat3(&m_vPoints[i]);
		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % LINE_END]) - XMLoadFloat3(&m_vPoints[i]);
		vNormal = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);

		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(vNormal))))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			if(pOutNormal)
				XMStoreFloat3(pOutNormal, vNormal);
			return false;
		}
	}

	return true;
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDest))
			return true;
	}

	return false;
}

#ifdef _DEBUG

HRESULT CCell::Render()
{
	if(m_pVIBuffer != nullptr)
		return m_pVIBuffer->Render();
	if(m_pVIBufferLine != nullptr)
		return m_pVIBufferLine->Render();
}

#endif

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _int iIndex, POINT ePoint)
{
	CCell*		pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex, ePoint)))
	{
		MSG_BOX(TEXT("Failed to Created : CCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIBufferLine);
#endif
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
