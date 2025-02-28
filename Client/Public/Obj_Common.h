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

class CObj_Common final : public CObject
{
private:
	CObj_Common(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObj_Common(const CObj_Common& rhs);
	virtual ~CObj_Common() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CLoadModel* m_pModelCom = { nullptr };
	//CModel* m_pModelCom = { nullptr };

public:
	void setRotate(_float _dgree) {
		m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(_dgree));
	}

private:
	HRESULT Ready_Components(wstring sComponentName);
	HRESULT Bind_ShaderResources();

public:
	_vector GetPlayerPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

public:
	static CObj_Common* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END