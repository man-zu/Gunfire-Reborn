#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "UI_Etc_BoxItem.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_Etc_BoxItemsBg final : public CUI
{
private:
	CUI_Etc_BoxItemsBg(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Etc_BoxItemsBg(const CUI_Etc_BoxItemsBg& rhs);
	virtual ~CUI_Etc_BoxItemsBg() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };		
	CVIBuffer_Rect*					m_pVIBufferCom = { nullptr };
	_bool							m_bShow = { false };
	CUI_Etc_BoxItem::BOXITEM_DESC	m_bDesc = {};

private:
	HRESULT Ready_Components();

public:
	void SetShowYN(_bool bYN) { m_bShow = bYN; }

public:
	static CUI_Etc_BoxItemsBg* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END