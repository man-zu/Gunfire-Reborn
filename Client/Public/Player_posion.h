#pragma once

#include "Client_Defines.h"
#include "Object.h"

BEGIN(Engine)
class CShader;
class CLoadModel;
class CModel;
class CTexture;
class CTransform;
END

BEGIN(Client)

class CPlayer_posion final : public CObject
{
private:
	CPlayer_posion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_posion(const CPlayer_posion& rhs);
	virtual ~CPlayer_posion() = default;

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

	_float			m_fJumpTime = 0.f;
	_float			m_fGravity = 9.8f;
	_float			m_fJumpPower = 1.5f;
	_bool			m_bFall = false;

public:
	void setRotate(_float _dgree) {
		m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(_dgree));
	}

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	_vector GetPlayerPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

public:
	static CPlayer_posion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END