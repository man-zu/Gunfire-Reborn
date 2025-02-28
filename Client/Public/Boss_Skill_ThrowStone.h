#pragma once

#include "Client_Defines.h"
#include "Object.h"

BEGIN(Engine)
class CShader;
class CLoadModel;
class CModel;
class CCollider;
END

BEGIN(Client)

class CBoss_Skill_ThrowStone final : public CObject
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_uint iAttack = 0;
	}THROWSTONE;

private:
	CBoss_Skill_ThrowStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Skill_ThrowStone(const CBoss_Skill_ThrowStone& rhs);
	virtual ~CBoss_Skill_ThrowStone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CLoadModel*		m_pModelCom = { nullptr };
	//CModel*			m_pModelCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	_vector			m_OrginPos = {};

	class CPlayer*	m_pPlayer = { nullptr };

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
	static CBoss_Skill_ThrowStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END