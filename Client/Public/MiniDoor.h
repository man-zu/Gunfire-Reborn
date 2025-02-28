#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMiniDoor final : public CGameObject
{
public:
	enum DoorTB {TOP, BOTTOM, MINIDOOR_NONE};

public:
	typedef struct tDoor : public CTransform::TRANSFORM_DESC
	{
		DoorTB		eDoorType = MINIDOOR_NONE;
		_float		fScale = 1.f;
	}MINIDOOR_DESC;

private:
	CMiniDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMiniDoor(const CMiniDoor& rhs);
	virtual ~CMiniDoor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };
	_bool					m_bOpen = { false };
	_bool					m_bOpenTextShow = { false };
	DoorTB					m_eTB = { MINIDOOR_NONE };
	_uint					m_iMonRequireKillCnt = 0;
	_int					m_iMonCnt = 0;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void CheckColl();

public:
	static CMiniDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END