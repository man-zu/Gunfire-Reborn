#pragma once

#include "Client_Defines.h"
#include "UI.h"

#include "GameInstance.h"

BEGIN(Engine)
class CGameInstance;
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Crosshair final : public CUI
{
//DECLARE_SINGLETON(CUI_Crosshair)

private:
	CUI_Crosshair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Crosshair(const CUI_Crosshair& rhs);
	virtual ~CUI_Crosshair() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };		
	CTexture*			m_pTextureComBasic = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	static _bool		m_bFree;
	_bool				m_bGameLevel = false;
	_bool				m_bHide = false;

private:
	HRESULT Ready_Components();

public:
	void SetFree(_bool _b) { m_bFree = _b; }
	void SetHide(_bool _b) { m_bHide = _b; }

public:
	static CUI_Crosshair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END