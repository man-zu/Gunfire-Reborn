#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Weapon.h"
#include "Player.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CPartObject;
END

BEGIN(Client)

class CPlayer_LevelBoss : public CPlayer
{
public:
	enum PLAYERSTATE { STOP, MOVE };

	using PLAYER_DESC = struct TagPlayerDesc : public CTransform::TRANSFORM_DESC
	{
		LEVEL eLevel;
	};

protected:
	CPlayer_LevelBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_LevelBoss(const CPlayer_LevelBoss& rhs);
	virtual ~CPlayer_LevelBoss() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;

public:
	static CPlayer_LevelBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END