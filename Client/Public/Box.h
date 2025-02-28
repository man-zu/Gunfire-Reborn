#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "BoxPart.h"

BEGIN(Engine)
class CCollider;
class CPartObject;
END

BEGIN(Client)

class CBox final : public CGameObject
{
private:
	CBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBox(const CBox& rhs);
	virtual ~CBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CPartObject*>			m_Parts;
	CBoxPart*						m_pBody = { nullptr };
	_float4							m_pPosMove = { 0.f, 0.f, 0.f, 1.f };	// 몬스터 초기 위치 보정

private:
	HRESULT Ready_PartObjects(void* pArg);
	HRESULT Ready_Components();

public:
	CCollider* GetColl() { return m_pBody->GetColl(); }

public:
	static CBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END