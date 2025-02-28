#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect_Instance;
END


BEGIN(Client)

class CParticle_BoxOpen final : public CGameObject
{
private:
	CParticle_BoxOpen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_BoxOpen(const CParticle_BoxOpen& rhs);
	virtual ~CParticle_BoxOpen() = default;

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
	CVIBuffer_Rect_Instance*	m_pVIBufferCom = { nullptr };
	_int						m_pTextureColor = 0;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CParticle_BoxOpen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END