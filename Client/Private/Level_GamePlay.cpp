
#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
#include "UI_TextNum.h"
#include "Object.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext } 
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	m_iLevelID = LEVEL_TOOL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;
		
	///* 로딩 레벨에 보여주기위한 객체(배경, 로딩바, 로딩상태폰트)들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	//if (FAILED(Ready_Layer_PlayerWeapon(L"Layer_Gun")))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(L"Layer_Effect"))))
	//	return E_FAIL;


	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	/****************************************************/
	/*********************** UI *************************/
	/****************************************************/
	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(L"Layer_MainCamera")))
		return E_FAIL;


	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_GamePlay::Render()
{
	
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = CAMERA_FAR;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, L"Prototype_GameObject_Camera_Free", &CameraDesc)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, L"Prototype_GameObject_Camera_Main", &CameraDesc)))
	//	return E_FAIL;
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{	
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, L"Prototype_GameObject_Terrain")))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_PlayerWeapon(const wstring& strLayerTag)
{
	CTransform::TRANSFORM_DESC transDesc{};

	transDesc.fSpeedPerSec = 10.f;
	transDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Layer_Gun", L"Prototype_GameObject_Player_Basic", &transDesc)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Layer_Gun", L"Prototype_GameObject_Player_BlueEye", &transDesc)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Layer_Gun", L"Prototype_GameObject_Player_SR", &transDesc)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Layer_Gun", L"Prototype_GameObject_Player_Pistol", &transDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
	for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Monster"))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI()
{
	/* Player UI */
	if (FAILED(Ready_Layer_UI_Player(L"Layer_UI_PlayerIcon")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Player_Bg(L"Layer_UI_PlayerIcon_Bg")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Player_Hp_Bar(L"Layer_UI_Player_Hp_Bar")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Player_Shield_Bar(L"Layer_UI_Player_Shield_Bar")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_MiniMap_Bg(L"Layer_UI_MiniMap_Bg")))
		return E_FAIL;

	/*  */
	if (FAILED(Ready_Layer_UI_Crosshair(L"Layer_Crosshair")))
		return E_FAIL;

	// 왼쪽정렬
	if (FAILED(Ready_Layer_UI_White_Num(L"Layer_UI_White_Num")))
		return E_FAIL;

	// 우측정렬

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Player(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, L"Prototype_GameObject_UI_PlayerIcon")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Player_Bg(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, L"Prototype_GameObject_UI_PlayerIcon_Bg")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Player_Hp_Bar(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, L"Prototype_GameObject_Player_HP_Bar")))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Player_Shield_Bar(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, L"Prototype_GameObject_Player_Shield_Bar")))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_MiniMap_Bg(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, L"Prototype_GameObject_UI_MiniMap")))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Crosshair(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, L"Prototype_GameObject_UI_Crosshar")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_White_Num(const wstring& strLayerTag)
{
	//CUI_TextNum::NUM_DESC tNumDesc;
	//tNumDesc.fSizeX = 20.f;
	//tNumDesc.fSizeY = 20.f;
	//tNumDesc.fX = -(g_iWinSizeX * 0.5f) + 1000.f;
	//tNumDesc.fY = (g_iWinSizeY * 0.5f) - 620.f;
	//
	//_wstring sNumText = L"1234567890";
	//
	//for (int i = 0; i < sNumText.length(); ++i)
	//{
	//	tNumDesc.fX += 20.f;
	//	tNumDesc.sNum = sNumText[i];
	//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, strLayerTag, L"Prototype_GameObject_UI_White_Num", &tNumDesc)))
	//		return E_FAIL;
	//}

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
