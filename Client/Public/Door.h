#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CDoor final : public CGameObject
{
public:
	enum DoorLR {LEFT, RIGHT, DOOR_NONE};

public:
	typedef struct tDoor : public CTransform::TRANSFORM_DESC
	{
		DoorLR		eDoorType = DOOR_NONE;
		_float		fScale = 1.f;
		_float		fTurn = 0.f;
	}DOOR_DESC;

private:
	CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoor(const CDoor& rhs);
	virtual ~CDoor() = default;

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
	class CPlayer*			m_pPlayer = { nullptr };
	_bool					m_bTurn = { false };
	_bool					m_bOpen = { false };
	DoorLR					m_eLF = { DOOR_NONE };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	_bool IsDoorOpen() { return m_bOpen; }

public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END