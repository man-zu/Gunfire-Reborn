#pragma once

#include "Client_Defines.h"
#include "Level.h"


BEGIN(Client)

class CLevel_Stage1 final : public CLevel
{
private:
	CLevel_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Stage1() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:

	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);

	HRESULT Ready_Lights();

	HRESULT Ready_LandObjects();
	HRESULT Ready_Layer_Player(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);

	HRESULT Ready_Layer_UI();
	HRESULT Ready_Layer_UI_Player(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI_Player_Hp_Bar(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI_Player_Shield_Bar(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI_MiniMap_Bg(const wstring& strLayerTag);

	HRESULT Ready_Layer_UI_White_Num(const wstring& strLayerTag);
	
	/* 실제 게임플레이 화면의 배경을 구성하는데 있어 필요한 객체들을 모아놓은 레이어를 생성한다. */

private:
	_bool			m_bStageStep3 = false;


public:
	static CLevel_Stage1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
