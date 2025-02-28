#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CUI : public CGameObject
{
public:
	typedef enum UIObjType
	{
		UI_BACKGROUND, UI_LOADING, UI_BUTTON, UI_END

	}UITYPE;

	using UIDESC = struct tagUIDesc
	{
		wstring sTextureTag = L"";
		UITYPE eUIObjType = { UI_END };
		_float fSizeX = { 0.f };
		_float fSizeY = { 0.f };
		_float fX = { 0.f };
		_float fY = { 0.f };
		_float fZ = { 0.f };
		_float fRot = { 0.f };
		_uint iTextureIdx = {0};
	};

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

protected:
	UIDESC						m_tUIDesc = {};
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

public:
	virtual void Free() override;
};

END