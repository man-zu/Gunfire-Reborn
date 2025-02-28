#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect_Instance;
END


BEGIN(Client)

class CParticle_BossLevel final : public CGameObject
{
private:
	CParticle_BossLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_BossLevel(const CParticle_BossLevel& rhs);
	virtual ~CParticle_BossLevel() = default;

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
	_float						m_pTextureIdx = 0;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CParticle_BossLevel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END