#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers();
	virtual HRESULT Render() override;

public:
	

protected:
	ID3D11Buffer*					m_pVB = { nullptr };
	ID3D11Buffer*					m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC				m_BufferDesc = {};
	D3D11_SUBRESOURCE_DATA			m_InitialData = {};
	_uint							m_iVertexStride = {};
	_uint							m_iIndexStride = {};
	_uint							m_iNumVertices = {};
	_uint							m_iNumIndices = {};
	_uint							m_iNumVertexBuffers = {};
	DXGI_FORMAT						m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY		m_ePrimitiveTopology = {};

	_float3*						m_pVertexPositions = { nullptr } ;





protected:
	HRESULT Create_Buffer(ID3D11Buffer** ppOut);


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END