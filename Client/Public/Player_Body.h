#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer_Body final : public Engine::CPartObject
{
public:

private:
	CPlayer_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Body(const CPlayer_Body& rhs);
	virtual ~CPlayer_Body() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	_float4x4				m_SavedMat = {};

public:
	void SaveCurMatix() { XMStoreFloat4x4(&m_SavedMat, m_pTransformCom->Get_WorldMatrix()); }
	void SetSavedMatix() { m_pTransformCom->Set_WorldMat(m_SavedMat); m_fDelayTime = 0.f; }

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END