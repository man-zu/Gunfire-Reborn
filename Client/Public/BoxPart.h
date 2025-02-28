#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CBoxPart final : public CPartObject
{
private:
	CBoxPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoxPart(const CBoxPart& rhs);
	virtual ~CBoxPart() = default;

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
	CCollider*				m_pColliderCom = { nullptr };
	_int					m_iMonCnt = 0;
	_bool					m_bOpenTextShow = { false };
	_bool					m_bOpen = { false };
	_uint					m_iStateCurNum = 0;
	class CPlayer*			m_pPlayer = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void openBox(_float fTimeDelta);
	void CheckColl();

public:
	CCollider* GetColl() { return m_pColliderCom; }

public:
	static CBoxPart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END