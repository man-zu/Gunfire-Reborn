#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CPartObject;
END

BEGIN(Client)

class CMonster : public CGameObject
{
public:
	typedef struct TagMonsterDesc : public CPartObject::PARTOBJ_DESC
	{
		LEVEL eLevel{ LEVEL_END };
	}MONSTER_DESC;

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	_bool		m_bRim{ false };
	_bool		m_bDieEffect{ false };

public:
	void		SetRim(_bool b) { m_bRim = b; }

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END