#pragma once

#include "Client_Defines.h"
#include "Object.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBoss_Skill_Shot final : public CGameObject
{

private:
	CBoss_Skill_Shot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Skill_Shot(const CBoss_Skill_Shot& rhs);
	virtual ~CBoss_Skill_Shot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

	class CPlayer*	m_pPlayer = { nullptr };

	_float			m_fFrame = { 0.f };
	_float3			m_OrginScale = {0.f, 0.f, 0.f};
	_vector			m_TargetDir = {};
	_float			m_fSize = .000001f;

public:
	void setRotate(_float _dgree) {
		m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(_dgree));
	}

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	virtual void CheckColl() override;

public:
	_vector GetPlayerPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

public:
	static CBoss_Skill_Shot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END