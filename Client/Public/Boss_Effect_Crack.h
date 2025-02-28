#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CCollider;
class CTexture;
class CShader;
END

BEGIN(Client)

class CBoss_Effect_Crack final : public CGameObject
{
private:
	CBoss_Effect_Crack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Effect_Crack(const CBoss_Effect_Crack& rhs);
	virtual ~CBoss_Effect_Crack() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

	_bool				m_bDamaged = { false };
	_float				m_fAlphaChange = { 0.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void CheckColl();

public:
	static CBoss_Effect_Crack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END