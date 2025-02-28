#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CCollider;
class CShader;
END

BEGIN(Client)

class CMonster_Effect final : public CGameObject
{
public:
	enum eEffectType { GREYFOG, ITEM, EFFECTTYPE_END };

public:
	typedef struct tEffectInfo : public CTransform::TRANSFORM_DESC
	{
		eEffectType		eEffectType = EFFECTTYPE_END;
		CNavigation*	pNavi = nullptr;
	}MON_EFFECT_DESC;

private:
	CMonster_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Effect(const CMonster_Effect& rhs);
	virtual ~CMonster_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	void MakeGrayFog();
	void MakeItem(CNavigation* pNavigationCom);

private:
	CCollider*				m_pColliderCom = { nullptr };

	_float					m_fFrame = {};
	eEffectType				m_eEffectType = { EFFECTTYPE_END };
	_float4					m_pInitPos = {};
	_float					iRandX = 0.f;
	_float					iRandZ = 0.f;
	_int					iSign = 0;

private:
	HRESULT Ready_Components();
	virtual void CheckColl() override;

public:
	static CMonster_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END