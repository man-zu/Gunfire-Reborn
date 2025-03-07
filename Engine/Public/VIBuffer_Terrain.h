#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath, _uint iX, _uint iZ);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_uint				m_iNumVerticesX = {};
	_uint				m_iNumVerticesZ = {};

public:
	virtual _bool Picking(class CTransform* pTransform, _float3* pOut);

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath, _uint iX, _uint iZ);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END