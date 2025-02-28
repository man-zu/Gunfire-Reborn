#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{

}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent ( rhs )
	, m_pVB{ rhs.m_pVB }
	, m_pIB{ rhs.m_pIB }
	, m_iVertexStride{ rhs.m_iVertexStride }
	, m_iIndexStride{ rhs.m_iIndexStride }
	, m_iNumVertices{ rhs.m_iNumVertices }
	, m_iNumIndices{ rhs.m_iNumIndices }
	, m_iNumVertexBuffers{ rhs.m_iNumVertexBuffers }
	, m_eIndexFormat{ rhs.m_eIndexFormat }
	, m_ePrimitiveTopology{ rhs.m_ePrimitiveTopology }
	, m_pVertexPositions{ rhs.m_pVertexPositions }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	ID3D11Buffer* pVertexBuffers[] = {
m_pVB,
	};

	_uint					iStrides[] = {
		m_iVertexStride,
	};

	_uint					iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	/* 장치가 알아서 정점의 위치를 변환, 노말로 빛연산, 텍스쿠드로 샘플링하는 작업을 스스로 수행하지않는다. */
	/* 내 정점이 어떻게 생겼는지를 굳이 전달해줄 필요는 없다. */
	/* 위 작업응ㄹ 내가 직접 수행행햐나다. == 셰이더를 이용해야한ㄴ다. */
	/* 검증필요해!! => 내가 그릴려고하는 정점이 내가 이용할려고 하는 셰이더에 입력이 가능한지? */
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
	
	return S_OK;
}

HRESULT CVIBuffer::Bind_Buffers()
{
	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB,
	};

	_uint					iStrides[] = {
		m_iVertexStride,
	};

	_uint					iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	
	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppOut)
{
	/* 1. D3D11_BUFFER_DESC : 버퍼를 생성하기위한 설정 값들(몇바이트할당한건지? 속성은 정, 동적. */
	/* 2. D3D11_SUBRESOURCE_DATA : 지금 생성한 공간에 어떤 값들을 채워넣을지? */
	/* 3. Out */
	m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialData, ppOut);

	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pVertexPositions);
	
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
