#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CPlayer_Effect_Dash final : public CGameObject
{
private:
	CPlayer_Effect_Dash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Effect_Dash(const CPlayer_Effect_Dash& rhs);
	virtual ~CPlayer_Effect_Dash() = default;

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

private:
	_float						m_fX = {};
	_float						m_fY = {};
	_float						m_fSizeX = {};
	_float						m_fSizeY = {};
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

	_float						m_pDelayTime = { 0.f };

	_uint m_iIdx = { 0 };
	_bool m_bShow = { false };

public:
	void ShowDashEffect() { m_bShow = true; };

private:
	HRESULT Ready_Components();

public:
	static CPlayer_Effect_Dash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END