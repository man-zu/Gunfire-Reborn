#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_Etc_BoxItem final : public CUI
{
public:
	typedef struct tagBoxItem : public UIDESC
	{
		_float		fMargin = 0.f;
		_uint		iIdx = 0;
		_wstring	sItemName = L"";
		_wstring	sItemInfo = L"";
	} BOXITEM_DESC;

private:
	CUI_Etc_BoxItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Etc_BoxItem(const CUI_Etc_BoxItem& rhs);
	virtual ~CUI_Etc_BoxItem() = default;

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
	_bool						m_bShow = { false };
	_bool						m_bCreateItem = { false };
	vector<BOXITEM_DESC>		m_bItemVec = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_Items();

public:
	void SetShowYN(_bool bYN) { m_bShow = bYN; }

public:
	static CUI_Etc_BoxItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END