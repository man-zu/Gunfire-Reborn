#pragma once

#include "Client_Defines.h"
#include "Object.h"

BEGIN(Engine)
class CShader;
class CLoadModel;
class CCollider;
END

BEGIN(Client)

class CBoss_Skill_JumpStone final : public CObject
{
private:
	CBoss_Skill_JumpStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Skill_JumpStone(const CBoss_Skill_JumpStone& rhs);
	virtual ~CBoss_Skill_JumpStone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*	m_pShaderCom = { nullptr };
	CLoadModel* m_pModelCom = { nullptr };
	CCollider*	m_pColliderCom = { nullptr };

	_bool		m_bColl = { false };
	_float		m_fDissolveTime = 0.f;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void CheckColl() override;

public:
	_vector GetPlayerPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

public:
	static CBoss_Skill_JumpStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END