#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CObject : public CGameObject
{
public:
	typedef enum ObjType
	{
		OBJ_MAP, OBJ_ETC, OBJ_END

	}UITYPE;

	using OBJDESC = struct tagObjDesc : public CTransform::TRANSFORM_DESC
	{
		string sFileName = "";
		string sLayerTag = "";
		string sPrototpyeName = "";
		string sComponentName = "";
		UITYPE eObjType = { OBJ_END };
		_float fPosX = { 0.f };
		_float fPosY = { 0.f };
		_float fPosZ = { 0.f };
		_float fRotY = { 0.f };
		_float fX = { 0.f };
		_float fY = { 0.f };
	};

protected:
	CObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObject(const CObject& rhs);
	virtual ~CObject() = default;

protected:
	OBJDESC					m_tOBJDesc = {};


public:
	virtual void Free() override;
};

END