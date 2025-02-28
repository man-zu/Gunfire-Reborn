#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CVIBuffer_Rect;
class CTexture;
class CShader;
END

BEGIN(Client)

class CNextPortal final : public CGameObject
{
private:
	CNextPortal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNextPortal(const CNextPortal& rhs);
	virtual ~CNextPortal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };
	_bool					m_bPortal = { false };
	_bool					m_bOpenTextShow = { false };
	_int					m_iMonCnt = 0;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void CheckColl();

public:
	_bool GetOpenYN() { return m_bOpenTextShow; }

public:
	static CNextPortal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END