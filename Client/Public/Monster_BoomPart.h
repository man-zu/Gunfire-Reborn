#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Model.h"


BEGIN(Engine)
class CNavigation;
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster_BoomPart final : public Engine::CPartObject
{
private:
	CMonster_BoomPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_BoomPart(const CMonster_BoomPart& rhs);
	virtual ~CMonster_BoomPart() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);
	const _uint Get_CurStateIdx() { return m_pModelCom->getCurAnimIdx(); }
	BOOM_PTRN GetCurBoomState() { return m_eBoomState; }
	void SetCurBoomState(BOOM_PTRN _boomState) { m_eBoomState = _boomState; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CNavigation*			m_pNavigationCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };

	_float4					m_pPosMove = { 0.f, 0.f, 0.f, 1.f };	// 초기 위치 보정

	BOOM_PTRN				m_eBoomState = { BOOM_PTRN::BOOM_END };
	_uint					m_iStateCurNum = 0;
	_bool					m_bRim{ false };

#ifdef _DEBUG
	_uint m_tmp = 0;
#endif

private:
	HRESULT Ready_Components();
	HRESULT Ready_Navi(void* pArg);
	HRESULT Bind_ShaderResources();

private:
	void motion_moveToPlayer(_float fTimeDelta);
	void motion_boom();
	void motion_idle();
	void motion_reset();
	void CheckColl();

public:
	CCollider* GetColl() { return m_pColliderCom; }
	CNavigation* GetNavigation() { return m_pNavigationCom; }

public:
	static CMonster_BoomPart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END