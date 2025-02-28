#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_Etc_NextStage final : public CUI
{
private:
	CUI_Etc_NextStage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Etc_NextStage(const CUI_Etc_NextStage& rhs);
	virtual ~CUI_Etc_NextStage() = default;

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

private:
	HRESULT Ready_Components();

public:
	void SetShowYN(_bool bYN) { m_bShow = bYN; }

public:
	static CUI_Etc_NextStage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END