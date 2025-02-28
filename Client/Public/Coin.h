#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CCoin final : public CGameObject
{
public:
	typedef struct tCOIN : public CTransform::TRANSFORM_DESC
	{
		_uint		iCurrentCellIndex = 0;
		_bool		bAuto = false;
	}COIN_DESC;

private:
	CCoin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCoin(const CCoin& rhs);
	virtual ~CCoin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void CheckColl() override;

private:
	CNavigation*			m_pNavigationCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	_bool					m_bPop = true;
	_float					m_fJumpTime = 0.f;
	_float					iRandX = 0.f;
	_float					iRandZ = 0.f;
	_int					iSign  = 0;
	class CPlayer*			m_pPlayer = { nullptr };

private:
	void PopMotion(_float fTimeDelta);
	void moveToPlayer(_float fTimeDelta);

private:
	HRESULT Ready_Components(_uint iCellIdx);
	HRESULT Bind_ShaderResources();

public:
	static CCoin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END