#include "Navigation.h"
#include "Cell.h"


#include "GameInstance.h"

_float4x4	CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent( rhs )
	, m_Cells { rhs.m_Cells }
	, m_iCurrentIndex { rhs.m_iCurrentIndex }
#ifdef _DEBUG
	, m_pShader { rhs.m_pShader }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const _wstring & strNavigationDataFile)
{
	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[CCell::POINT_END] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size(), CCell::POINT_END);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);
	}

	CloseHandle(hFile);

	SetUp_Neighbors();

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize_Prototype()
{

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		CNavigation::NAVIGATION_DESC*	pDesc = static_cast<CNavigation::NAVIGATION_DESC*>(pArg);

		m_iCurrentIndex = pDesc->iCurrentCellIndex;
	}

	return S_OK;
}

HRESULT CNavigation::Render()
{

#ifdef _DEBUG

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (-1 == m_iCurrentIndex)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
			pCell->Render();
	}
	else
	{
		_float4x4		WorldMatrix = m_WorldMatrix;
		WorldMatrix.m[3][1] += 0.1f;

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &_float4(1.f, 0.0f, 0.f, 1.f), sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		m_Cells[m_iCurrentIndex]->Render();
	}
	

#endif

	return S_OK;
}

HRESULT CNavigation::Tick()
{

	return S_OK;
}

_bool CNavigation::isMove(_fvector vPosition, _int* iOutNeighborIndex, _float3* vOutNormal)
{
	if (-1 == m_iCurrentIndex || m_iCurrentIndex >= m_Cells.size())
		return false;

	/* Cell안에 있는 점 세개가 로컬에 존재, 인자로 받아온 vPosition 월드 위치. 싱크를 맞춰야된다. */
	/* vPosition -> LocalPos로 바꾸는 작업을 수행하자. */
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	/* 움직이고난 결과가 현재 존재한던 셀안에존재한다. == 현재 셀 안에서 움직인거다. */
	if (true == m_Cells[m_iCurrentIndex]->isIn(vLocalPos, iOutNeighborIndex, vOutNormal))
		return true;

	else /* 현재 셀을 벗어났다. */
	{
		/* 벗어난 방향에 이웃이 없으면. */
		if(-1 == *iOutNeighborIndex)		// 셀 안에 없고, 이웃X.		// 점밖을 아예 나갔을 경우 판단
		{
			if (true == m_Cells[m_iCurrentIndex]->isIn(vPosition, iOutNeighborIndex, vOutNormal))
				*iOutNeighborIndex = -2;		// -2: 셀 안에 없고, 이웃X. 내가 가려는 방향에 이웃이 있는 경우, 현 위치가 in 인가?

			return false;
		}

		/* 벗어난 방향에 이웃이 있으면. */
		else
		{
			while (true)
			{
				if (-1 == *iOutNeighborIndex)
				{
					//*iOutNeighborIndex = -2;		// -2: 셀 안에 없고, 이웃O.
					return false;
				}

				if (true == m_Cells[*iOutNeighborIndex]->isIn(vLocalPos, iOutNeighborIndex, vOutNormal))
					break;
			}		

			m_iCurrentIndex = *iOutNeighborIndex;
			return true;
		}		
	}
}

HRESULT CNavigation::AddCell(_float3 vPoint1, _float3 vPoint2)
{
	_float3 Cell[2] = {};
	Cell[0] = vPoint1;
	Cell[1] = vPoint2;
	 
	CCell* pCell = CCell::Create(m_pDevice, m_pContext, Cell, (_int)m_Cells.size(), CCell::POINT_C);
	if (nullptr == pCell)
		return E_FAIL;

	m_Cells.emplace_back(pCell);

	return S_OK;
}

HRESULT CNavigation::DeleteCell()
{
	vector<class CCell*>::iterator iter = m_Cells.end() - 3;

	for (int i = 0; i < 3; ++i)
	{
		Safe_Release(*iter);
		iter = m_Cells.erase(iter);
	}

	return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

_float CNavigation::GetHeight(_vector vPos, _int iIdx)
{
	_float4 Plane;
	_float fHeight;
	XMStoreFloat4(&Plane, XMPlaneFromPoints(m_Cells[iIdx]->Get_Point(CCell::POINT_A), m_Cells[iIdx]->Get_Point(CCell::POINT_B), m_Cells[iIdx]->Get_Point(CCell::POINT_C)));

	// 평면의 방정식 ax + by + cz + d = 0
	// by = -ax -cz - d
	fHeight = (-(XMVectorGetX(vPos) * Plane.x) - (XMVectorGetZ(vPos) * Plane.z) - Plane.w) / Plane.y;

	return fHeight;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _wstring & strNavigationDataFile)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;	
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
}
