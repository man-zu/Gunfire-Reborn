#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_PlayerShieldBar final : public CUI
{
private:
	CUI_PlayerShieldBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerShieldBar(const CUI_PlayerShieldBar& rhs);
	virtual ~CUI_PlayerShieldBar() = default;

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

	class CPlayer*				m_pPlayer = { nullptr };
	_float						m_fQuantity = 0.f;

private:
	HRESULT Ready_Components();

public:
	static CUI_PlayerShieldBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END