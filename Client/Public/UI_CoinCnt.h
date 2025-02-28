#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI_CoinCnt final : public CUI
{
private:
	CUI_CoinCnt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CoinCnt(const CUI_CoinCnt& rhs);
	virtual ~CUI_CoinCnt();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_uint				m_iNum = 0;
	class CPlayer*		m_pPlayer = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CUI_CoinCnt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END