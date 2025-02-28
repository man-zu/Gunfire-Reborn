#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Model.h"


BEGIN(Engine)
class CShader;
class CCollider;
END

BEGIN(Client)

class CBoss_65_Body final : public Engine::CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		BOSS_PTRN tBossState = { BOSS_INTRO_WAIT };
	} BODY_DESC;

private:
	CBoss_65_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_65_Body(const CBoss_65_Body& rhs);
	virtual ~CBoss_65_Body() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);
	const _uint Get_CurStateIdx() { return m_pModelCom->getCurAnimIdx(); }
	BOSS_PTRN GetCurBossState() { return m_eBossState; }
	void SetCurBossState(BOSS_PTRN eState) { m_eBossState = eState; }
	void SetNextBossState(BOSS_PTRN eState) { m_eBossNextState = eState; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };
	CCollider*				m_pColliderCom[4] = { nullptr, nullptr, nullptr, nullptr };

	_float4					m_PosMove = { 0.f, 0.f, 0.f, 1.f };		// 초기 위치 보정
	_float4					m_PosTarget = { 0.f, 0.f, 0.f, 1.f };	// (공격패턴) 공격할 위치

	BOSS_PTRN				m_eBossState = { BOSS_PTRN::BOSS_END };
	BOSS_PTRN				m_eBossNextState = { BOSS_PTRN::BOSS_END };
	BOSS_PTRN				m_eBossLastPtn = { BOSS_PTRN::BOSS_END };
	_uint					m_iStateCurNum = 0;

	_uint					m_iShotCnt = 0;


	/* 보스 패턴 순서대로 보여주기위한 변수 */
	_uint					m_iShowPtrn = 0;
	_bool					m_bCreateBool = { false };
	_bool					m_bSound = { false };

#ifdef _DEBUG
	_uint m_tmp = 0;
#endif

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void CheckColl();

private:
	void motion_intro(_float fTimeDelta);
	void motion_move(_float fTimeDelta);
	void motion_idle(_float fTimeDelta);
	void motion_attack_throw_rock(_float fTimeDelta);
	void motion_attack_run_jump(_float fTimeDelta);
	void motion_attack_run_punch(_float fTimeDelta);
	void motion_attack_run_shot(_float fTimeDelta);
	void motion_damaged(_float fTimeDelta);
	void motion_healing(_float fTimeDelta);
	void motion_die();
	void doNextPattern();

public:
	CCollider** GetColl() { return m_pColliderCom; }

public:
	static CBoss_65_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END