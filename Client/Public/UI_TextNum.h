#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI_TextNum final : public CUI
{
public:
	enum UITextureType { NUM_L, NUM_R, DASH, NUM };
public:
	using NUM_DESC = struct tNumDesc : public UIDESC
	{
		_uint			iDisit = 0;
		_wstring		sNum = L"";
		UITextureType	eTextureType{ NUM };
	};

private:
	CUI_TextNum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_TextNum(const CUI_TextNum& rhs);
	virtual ~CUI_TextNum();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_uint				m_iNum = 0;
	UITextureType		m_eTextureType = { NUM };

private:
	HRESULT Ready_Components();

public:
	static CUI_TextNum* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END