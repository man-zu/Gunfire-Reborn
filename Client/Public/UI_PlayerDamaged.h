#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_PlayerDamaged final : public CUI
{
private:
	CUI_PlayerDamaged(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerDamaged(const CUI_PlayerDamaged& rhs);
	virtual ~CUI_PlayerDamaged() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };		
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };

	_float						m_fAlphaChange = { 0.f };
	_bool						m_bUseYN = { false };

private:
	HRESULT Ready_Components();

public:
	void SetDamagedUI() { m_bUseYN = true; }

public:
	static CUI_PlayerDamaged* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END