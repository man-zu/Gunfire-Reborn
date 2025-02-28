#pragma once

#include "Transform.h"	

/*클라이언트개발자가 실질적으로 생성해낸 객체타입(Player, Monster)의 부모가 되는 클래스이다. */

/* LPDIRECT3DDEVICE9 == ID3D11Device + ID3D11DeviceContext */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	static const _wstring		m_strTransformTag;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_LightDepth() { return S_OK; }

protected:
	void ColDelay(_float fTimeDelta);

public:
	class CComponent* Find_Component(const wstring& strComponentTag);

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	class CTransform*		m_pTransformCom = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

	_bool					m_bIsDead = { false };
	_float					m_fDelayTime{ 0.f };
	
	_float					m_iMaxHp = { 0 };
	_float					m_iHp = { 0 };

	_float					m_fShieldChargeTime = { 0.f };
	_float					m_fChargeShield = { 0.f };
	_float					m_fMaxShield = { 0.f };
	_float					m_fShield = { 0.f };

	_float					m_fAttack = { 0.f };

	/* 충돌관련 */
	_float					m_fColDelay = 0.f;
	_bool					m_bCol = false;
	_bool					m_bDissolve{ false };

public:
	_float GetAttack() { return m_fAttack; }
	_float GetShield() { return m_fShield; }
	_float GetMaxShield() { return m_fMaxShield; }
	_float GetHp() { return m_iHp; }
	_float GetMaxHp() { return m_iMaxHp; }
	_bool CheckDead() { return m_bIsDead; }
	void Set_Dead(_bool bDead) { m_bIsDead = bDead; }

	void Attacked(_float iTargetAttPwr) {

		if (m_fShield <= 0.f)
		{
			m_iHp -= iTargetAttPwr;
		}

		if (m_fShield < iTargetAttPwr)
		{
			_float remainAttPwr = iTargetAttPwr - m_fShield;
			m_fShield = 0.f;

			if (m_iHp <= remainAttPwr)
				m_iHp = 0.f;
			else
				m_iHp -= remainAttPwr;
		}
		else
			m_fShield -= iTargetAttPwr;
	}

	_bool  ShieldCharge(_float fTimeDelta)
	{
		if (m_iHp <= 0.1f)
			return true;

		if (m_fShield >= m_fMaxShield)
		{
			m_fShield = m_fMaxShield;
			m_fChargeShield = 0.f;
			m_fShieldChargeTime = 0.f;
			return true;
		}

		m_fShieldChargeTime += fTimeDelta;
		if (m_fShieldChargeTime <= 60.f)
			return false;

		/* Shield bar 채우기 */
		if (m_fChargeShield == 0.f && m_fShield == m_fChargeShield)
		{
			m_fChargeShield = m_fMaxShield;
		}

		m_fShield += 0.5f;
		m_fChargeShield -= 0.5f;

		return false;
	}

protected:
	map<const wstring, class CComponent*>		m_Components;

protected:
	HRESULT Add_Component(_uint iPrototoypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);
	virtual void CheckColl();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END