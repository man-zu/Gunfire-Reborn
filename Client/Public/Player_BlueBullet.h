#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CCollider;
class CShader;
END

BEGIN(Client)

class CPlayer_BlueBullet final : public CGameObject
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_float fAttack = 0;
		_vector	vTargetPos = {};
	} PLAYER_BLUEBULLET;

private:
	CPlayer_BlueBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_BlueBullet(const CPlayer_BlueBullet& rhs);
	virtual ~CPlayer_BlueBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCollider*				m_pColliderCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

	_float					m_fFrame = {};
	_vector					TargetDir = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void CheckColl() override;

	void CollEffect();

public:
	static CPlayer_BlueBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END