#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI.h"
#include "UI_TextNum.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI_Com_Number final : public CUI
{

private:
	CUI_Com_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Com_Number(const CUI_Com_Number& rhs);
	virtual ~CUI_Com_Number();

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
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	_int				m_iNum = 0;
	_uint				m_iDigit = 0;
	CUI_TextNum::UITextureType		m_eTextureType = { CUI_TextNum::NUM };

private:
	HRESULT Ready_Components();

//public:
//	void	Bind_WitheNum(_wstring param) { m_sNumText = param; }

public:
	static CUI_Com_Number* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END