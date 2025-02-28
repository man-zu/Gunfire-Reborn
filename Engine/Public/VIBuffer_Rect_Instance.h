#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instance
{
private:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& rhs);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Update_Spread(_float fTimeDelta);
	virtual void Update_GoUp(_float fTimeDelta);
	// void Update_Fontain(_float fTimeDelta);



private:
	VTXMATRIX*				m_pInstanceVertices = { nullptr };

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END