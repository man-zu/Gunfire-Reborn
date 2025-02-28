
#include "Level_Stage1_Boss.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
#include "UI_TextNum.h"
#include "Object.h"
#include "Terrain.h"
#include "Player.h"
#include "Door.h"
#include "Effect_Common.h"

#include "Level_Loading.h"

CLevel_Stage1_Boss::CLevel_Stage1_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext } 
{

}

HRESULT CLevel_Stage1_Boss::Initialize()
{
	m_iLevelID = LEVEL_STAGE1_BOSS;
		
	/* 로딩 레벨에 보여주기위한 객체(배경, 로딩바, 로딩상태폰트)들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(L"Layer_Effect")))
		return E_FAIL;


	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	/****************************************************/
	/*********************** UI *************************/
	/****************************************************/
	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(L"Layer_MainCamera")))
		return E_FAIL;

	STOP(L"maze");
	PLAY(L"Boss", true);

	return S_OK;
}

void CLevel_Stage1_Boss::Tick(_float fTimeDelta)
{
	if(!m_bBossUI)
	{
		list<CGameObject*>* tmp = GET_LIST(LEVEL_STAGE1_BOSS, L"Layer_door");
		CDoor* ttmp = dynamic_cast<CDoor*>(tmp->front());

		if(ttmp->IsDoorOpen())
			Ready_Boss_UI();
	}
}

HRESULT CLevel_Stage1_Boss::Render()
{
	
	SetWindowText(g_hWnd, TEXT("[ BOSS ]"));

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_Camera_Free", &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	CTerrain::TERRAIN_DESC		terrainDesc{};
	terrainDesc.eLevel = LEVEL_STAGE1_BOSS;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_Terrain", &terrainDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_Map_Stage1_Boss")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_Effect(const wstring & strLayerTag)
{
	CTransform::TRANSFORM_DESC transDesc{};
	transDesc.vInitPos = {0.f, 0.f, 0.f, 1.f};

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_Boss_Effect_Crack", &transDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_Particle_BossLevel", L"Prototype_GameObject_Particle_BossLevel")))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(.7f, .7f, .7f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(10.f, 3.f, 10.f, 1.f);
	LightDesc.fRange = 10.f;

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);


	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_LandObjects()
{
	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;

	CDoor::DOOR_DESC doorDesc{};
	doorDesc.eDoorType = CDoor::LEFT;
	doorDesc.vInitPos = { 51.85f, 8.25f, 43.6f, 1.f };

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_door", L"Prototype_GameObject_Door", &doorDesc)))
		return E_FAIL;

	doorDesc.eDoorType = CDoor::RIGHT;
	doorDesc.vInitPos = { 58.55f, 8.25f, 42.7f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_door", L"Prototype_GameObject_Door", &doorDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_Player(const wstring& strLayerTag)
{
	CPlayer::PLAYER_DESC playerDesc{};

	playerDesc.fSpeedPerSec = 5.f;
	playerDesc.fRotationPerSec = XMConvertToRadians(180.0f);
	playerDesc.vInitPos = { 54.f, 1.f, 6.3f, 1.f };
	playerDesc.eLevel = LEVEL_STAGE1_BOSS;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, L"Prototype_GameObject_Player", &playerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_Monster(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, TEXT("Prototype_GameObject_Boss"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_UI()
{
	/* Player UI */
	if (FAILED(Ready_Layer_UI_Player(L"Layer_UI_PlayerIcon")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Player_Hp_Bar(L"Layer_UI_Player_Hp_Bar")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Player_Shield_Bar(L"Layer_UI_Player_Shield_Bar")))
		return E_FAIL;

	//if (FAILED(Ready_Layer_UI_MiniMap_Bg(L"Layer_UI_MiniMap_Bg")))
	//	return E_FAIL;

	// 왼쪽정렬
	if (FAILED(Ready_Layer_UI_White_Num(L"Layer_UI_WeaponNum")))
		return E_FAIL;

	CUI::UIDESC tUiDesc;



	/********************************************** Player Weapon UI **********************************************/
	/* Weapon */
	tUiDesc.fSizeX = 256.f;
	tUiDesc.fSizeY = 81.f * 0.8f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 1140.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 630.f;
	tUiDesc.fZ = 0.1f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Weapon_bg";

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_PlayerWeapon_bg", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	/* UI SKILL MID BG */
	tUiDesc.fSizeX = 64.f * 1.2f;
	tUiDesc.fSizeY = 64.f * 1.2f;
	tUiDesc.fX = 0.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 630.f;
	tUiDesc.fZ = 0.1f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Weapon_Skill_bg0";

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_PlayerWeapon_bg", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;
	/********************************************** Player Weapon UI END **********************************************/


	/********************************************** Player Skill UI **********************************************/
	/* UI SKILL RIGHT BG */
	/* 왼쪽 스킬 독연막(E) */
	tUiDesc.fSizeX = 64.f * 1.2f;
	tUiDesc.fSizeY = 64.f * 1.2f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 900.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 630.f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Weapon_Skill_bg1";
	tUiDesc.iTextureIdx = 1;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_PlayerWeapon_bg", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_PlayerWeapon_SkillR", L"Prototype_GameObject_UI_PlayerWeapon_SkillbgR", &tUiDesc)))
		return E_FAIL;

	/* UI SKILL RIGHT BG */
	/* 오른쪽 스킬 Shift */
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 980.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 630.f;
	tUiDesc.iTextureIdx = 2;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_PlayerWeapon_bg", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_PlayerWeapon_SkillR", L"Prototype_GameObject_UI_PlayerWeapon_SkillbgR", &tUiDesc)))
		return E_FAIL;

	/* 가운데 스킬 보라색(Q) */
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_PlayerWeapon_SkillMid", L"Prototype_GameObject_UI_PlayerWeapon_SkillbgMid")))
		return E_FAIL;

	/********************************************** Player Skill UI END **********************************************/


	/********************************************** Player Weapon UI **********************************************/
	/* Cur Weapon */
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_PlayerWeapon_Img", L"Prototype_GameObject_UI_PlayerWeapon_Img")))
		return E_FAIL;
	/********************************************** Player Weapon UI END **********************************************/


	/********************************************** Player Coin UI **********************************************/
	/* UI Coin Icon */
	tUiDesc.fSizeX = 91.f * .5f;
	tUiDesc.fSizeY = 86.f * .5f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 1050.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 50.f;
	tUiDesc.fZ = 0.1f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_PropCoin";

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_Coin", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	/* Cur CoinCntNum */
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_CoinCnt", L"Prototype_GameObject_UI_CoinCnt")))
		return E_FAIL;
	/********************************************** Player Coin UI END **********************************************/


	tUiDesc.fSizeX = 256.f;
	tUiDesc.fSizeY = 73.f * 0.8f;
	tUiDesc.fX = 0.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 500.f;
	tUiDesc.fZ = 0.f;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_Player_Restart", L"Prototype_GameObject_UI_Player_Restart", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_NextStage", L"Prototype_GameObject_UI_Etc_NextStage", &tUiDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_UI_Player(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_UI_PlayerIcon")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_UI_PlayerIcon_Bg")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Player_UI_Damaged", L"Prototype_GameObject_Player_Damaged")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_UI_Player_Hp_Bar(const wstring& strLayerTag)
{
	CUI::UIDESC tUiDesc;

	tUiDesc.fSizeX = 150.f;
	tUiDesc.fSizeY = 8.f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 253.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 653.f;
	tUiDesc.fZ = 0.3f;
	//tUiDesc.fRot = 2.f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Character_HPBar3";

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_Player_HP_Bar")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_UI_Player_Shield_Bar(const wstring& strLayerTag)
{
	CUI::UIDESC tUiDesc;

	tUiDesc.fSizeX = 150.f;
	tUiDesc.fSizeY = 8.f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 258.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 639.f;
	tUiDesc.fZ = 0.3f;
	//tUiDesc.fRot = 2.f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Character_HPBar3";

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_Player_Shield_Bar")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_UI_MiniMap_Bg(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_UI_MiniMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_UI_MiniMap_Player")))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_UI_Crosshair(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, strLayerTag, L"Prototype_GameObject_UI_Crosshar")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_UI_White_Num(const wstring& strLayerTag)
{
	CUI_TextNum::NUM_DESC tNumDesc;
	tNumDesc.fSizeX = 20.f;
	tNumDesc.fSizeY = 20.f;
	tNumDesc.fX = -(g_iWinSizeX * 0.5f) + 1100.f;
	tNumDesc.fY = (g_iWinSizeY * 0.5f) - 570.f;
	tNumDesc.eTextureType = CUI_TextNum::NUM_L;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_WeaponNum_Mag", L"Prototype_GameObject_UI_Weapon_CountNum", &tNumDesc)))
		return E_FAIL;

	tNumDesc.fSizeX = 20.f;
	tNumDesc.fSizeY = 20.f;
	tNumDesc.fX = -(g_iWinSizeX * 0.5f) + 1200.f;
	tNumDesc.fY = (g_iWinSizeY * 0.5f) - 570.f;
	tNumDesc.eTextureType = CUI_TextNum::NUM_R;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_WeaponNum_Tot", L"Prototype_GameObject_UI_Weapon_CountNum", &tNumDesc)))
		return E_FAIL;

	tNumDesc.fSizeX = 20.f;
	tNumDesc.fSizeY = 20.f;
	tNumDesc.fX = -(g_iWinSizeX * 0.5f) + 1120.f;
	tNumDesc.fY = (g_iWinSizeY * 0.5f) - 570.f;
	tNumDesc.eTextureType = CUI_TextNum::DASH;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_WeaponNum_Dash", L"Prototype_GameObject_UI_Weapon_CountNum", &tNumDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Boss_UI()
{
	m_bBossUI = true;

	CUI::UIDESC tUiDesc;
	/********************************************** BOSS UI **********************************************/
	tUiDesc.fSizeX = 128.f;
	tUiDesc.fSizeY = 122.f;
	tUiDesc.fX = -210.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 100.f;
	tUiDesc.fZ = 0.f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Boss_UI_Icon";

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_Boss_Icon", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_Boss_Bar", L"Prototype_GameObject_Boss65HPBar")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_Boss_Bar", L"Prototype_GameObject_Boss65ShieldBar")))
		return E_FAIL;

	tUiDesc.fSizeX = 530.f;
	tUiDesc.fSizeY = 59.f;
	tUiDesc.fX = -47.f + 80.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 100.f;
	tUiDesc.fZ = 0.4f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Boss_UI_Hp_Bg";

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_Boss_Icon", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;


	tUiDesc.fSizeX = 420.f;
	tUiDesc.fSizeY = 14.f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 215.f + 390.f + 80.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 110.f;
	tUiDesc.fZ = 0.3f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Character_HPBar3";

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_Boss_Icon", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	tUiDesc.fSizeX = 420.f;
	tUiDesc.fSizeY = 8.f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 225.f + 388.f + 80.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 95.f;
	tUiDesc.fZ = 0.3f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Character_HPBar3";

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1_BOSS, L"Layer_UI_Boss_Icon", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	/********************************************** BOSS UI END **********************************************/
	return S_OK;
}

CLevel_Stage1_Boss * CLevel_Stage1_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage1_Boss*	pInstance = new CLevel_Stage1_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"Failed to Created : CLevel_Stage1_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage1_Boss::Free()
{
	__super::Free();

}
