
#include "Level_Stage2.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
#include "UI_TextNum.h"
#include "Object.h"
#include "Terrain.h"
#include "Player.h"
#include "MiniDoor.h"
#include "Monster.h"

#include "Level_Loading.h"
#include "Effect_Common.h"
#include "NextPortal.h"
#include "UI_Etc_NextStage.h"

CLevel_Stage2::CLevel_Stage2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext } 
{

}

HRESULT CLevel_Stage2::Initialize()
{
	m_iLevelID = LEVEL_STAGE2;

	if (FAILED(Ready_Lights()))
		return E_FAIL;
		
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

	return S_OK;
}

void CLevel_Stage2::Tick(_float fTimeDelta)
{
	_bool nextYN = dynamic_cast<CNextPortal*>(GET_LIST(LEVEL_STATIC, L"Layer_Portal")->front())->GetOpenYN();

	if (nextYN && KEY_PUSH(DIK_F))
	{
		PLAYONLY(L"NextStage", false);
		dynamic_cast<CUI_Etc_NextStage*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_NextStage")->front())->SetShowYN(false);
		dynamic_cast<CNextPortal*>(GET_LIST(LEVEL_STATIC, L"Layer_Portal")->front())->Set_Dead(true);

		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1_BOSS))))
			return;
		return;
	}
	if (KEY_PUSH(DIK_L))
	{
		dynamic_cast<CUI_Etc_NextStage*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_NextStage")->front())->SetShowYN(false);
		dynamic_cast<CNextPortal*>(GET_LIST(LEVEL_STATIC, L"Layer_Portal")->front())->Set_Dead(true);

		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1_BOSS))))
			return;
		return;
	}
}

HRESULT CLevel_Stage2::Render()
{
	
	SetWindowText(g_hWnd, TEXT("[ STAGE 2 ]"));

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Camera(const wstring & strLayerTag)
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

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, L"Prototype_GameObject_Camera_Free", &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_BackGround(const wstring & strLayerTag)
{	
	CTerrain::TERRAIN_DESC		terrainDesc{};
	terrainDesc.eLevel = (LEVEL)m_iLevelID;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, L"Prototype_GameObject_Terrain", &terrainDesc)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Effect(const wstring & strLayerTag)
{

	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, L"Prototype_GameObject_Player_Effect_Dash")))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Lights()
{
	//LIGHT_DESC			LightDesc{};
	//
	//LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	////LightDesc.vDiffuse = _float4(.55f, .55f, .55f, 1.f);
	//LightDesc.vDiffuse = _float4(.7f, .7f, .7f, 1.f);
	//LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	//
	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_LandObjects()
{
	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;

	CMiniDoor::MINIDOOR_DESC doorDesc{};
	doorDesc.eDoorType = CMiniDoor::TOP;
	doorDesc.vInitPos = { 38.3f, -1.1f, 34.16f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_MiniDoor", L"Prototype_GameObject_MiniDoor", &doorDesc)))
		return E_FAIL;

	doorDesc.eDoorType = CMiniDoor::BOTTOM;
	doorDesc.vInitPos = { 38.3f, -1.1f, 34.16f, 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_MiniDoor", L"Prototype_GameObject_MiniDoor", &doorDesc)))
		return E_FAIL;
	
	CTransform::TRANSFORM_DESC portalDesc{};
	portalDesc.vInitPos = { 39.6f, 1.f, 62.4f, 1.f };
	portalDesc.fTurn = 0;
	portalDesc.fSpeedPerSec = 1.f;
	portalDesc.fRotationPerSec = 1.f;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Portal", L"Prototype_GameObject_NextPortal", &portalDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Player(const wstring& strLayerTag)
{
	CPlayer::PLAYER_DESC playerDesc{};
	
	playerDesc.fSpeedPerSec = 5.f;
	playerDesc.fRotationPerSec = XMConvertToRadians(180.0f);
	playerDesc.vInitPos = { 7.7f, 1.f, 15.3f, 1.f };
	playerDesc.eLevel = (LEVEL)m_iLevelID;
	
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, L"Prototype_GameObject_Player", &playerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Monster(const wstring& strLayerTag)
{
	CMonster::MONSTER_DESC dsec{};
	dsec.eLevel = (LEVEL)m_iLevelID;

	//dsec.vInitPos = { 39.6f, 0.51f, 10.1f, 1.f };
	//dsec.iCurrentCellIndex = 65;
	//m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Boom", L"Prototype_GameObject_Monster_Boom", &dsec);
	dsec.vInitPos = { 27.6f, 1.5f, 4.3f, 1.f };
	dsec.iCurrentCellIndex = 50;
	m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Sword", L"Prototype_GameObject_Monster_Sword", &dsec);
	
	/* 화살 */
	dsec.vInitPos = { 24.6f, 2.0f, 15.7f, 1.f };
	dsec.iCurrentCellIndex = 36;
	m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Arrow", L"Prototype_GameObject_Monster_Arrow", &dsec);
	dsec.vInitPos = { 34.21f, 2.1f, 7.0f, 1.f };
	dsec.iCurrentCellIndex = 55;
	m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Arrow", L"Prototype_GameObject_Monster_Arrow", &dsec);
	dsec.vInitPos = { 18.5f, 1.5f, 20.9f, 1.f };
	dsec.iCurrentCellIndex = 29;
	m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Arrow", L"Prototype_GameObject_Monster_Arrow", &dsec);
	dsec.vInitPos = { 38.1f, 1.9f, 43.1f, 1.f };
	dsec.iCurrentCellIndex = 89;
	m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Arrow", L"Prototype_GameObject_Monster_Arrow", &dsec);
	
	
	/* 검 */
	dsec.vInitPos = { 39.1f, 0.5f, 4.18f, 1.f };
	dsec.iCurrentCellIndex = 66;
	m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Sword", L"Prototype_GameObject_Monster_Sword", &dsec);
	dsec.vInitPos = { 37.3f, -0.4f, 48.3f, 1.f };
	dsec.iCurrentCellIndex = 114;
	m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Sword", L"Prototype_GameObject_Monster_Sword", &dsec);
	dsec.vInitPos = { 37.7f, -0.48f, 55.2f, 1.f };
	dsec.iCurrentCellIndex = 114;
	m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Sword", L"Prototype_GameObject_Monster_Sword", &dsec);
	dsec.vInitPos = { 46.41f, -0.44f, 55.27f, 1.f };
	dsec.iCurrentCellIndex = 113;
	m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Sword", L"Prototype_GameObject_Monster_Sword", &dsec);
	dsec.vInitPos = { 28.2f, 1.6f, 5.6f, 1.f };
	dsec.iCurrentCellIndex = 50;
	m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Monster_Sword", L"Prototype_GameObject_Monster_Sword", &dsec);
	
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_UI()
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

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_PlayerWeapon_bg", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	/* UI SKILL MID BG */
	tUiDesc.fSizeX = 64.f * 1.2f;
	tUiDesc.fSizeY = 64.f * 1.2f;
	tUiDesc.fX = 0.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 630.f;
	tUiDesc.fZ = 0.1f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Weapon_Skill_bg0";

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_PlayerWeapon_bg", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;
	/********************************************** Player Weapon UI END **********************************************/


	/********************************************** Player Skill UI **********************************************/
	/* UI SKILL RIGHT BG */
	/* ���� ��ų ������(E) */
	tUiDesc.fSizeX = 64.f * 1.2f;
	tUiDesc.fSizeY = 64.f * 1.2f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 900.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 630.f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Weapon_Skill_bg1";
	tUiDesc.iTextureIdx = 1;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_PlayerWeapon_bg", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_PlayerWeapon_SkillR", L"Prototype_GameObject_UI_PlayerWeapon_SkillbgR", &tUiDesc)))
		return E_FAIL;

	/* UI SKILL RIGHT BG */
	/* ������ ��ų Shift */
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 980.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 630.f;
	tUiDesc.iTextureIdx = 2;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_PlayerWeapon_bg", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_PlayerWeapon_SkillR", L"Prototype_GameObject_UI_PlayerWeapon_SkillbgR", &tUiDesc)))
		return E_FAIL;

	/* ��� ��ų �����(Q) */
	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_PlayerWeapon_SkillMid", L"Prototype_GameObject_UI_PlayerWeapon_SkillbgMid")))
		return E_FAIL;

	/********************************************** Player Skill UI END **********************************************/


	/********************************************** Player Weapon UI **********************************************/
	/* Cur Weapon */
	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_PlayerWeapon_Img", L"Prototype_GameObject_UI_PlayerWeapon_Img")))
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

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_Coin", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	/* Cur CoinCntNum */
	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_CoinCnt", L"Prototype_GameObject_UI_CoinCnt")))
		return E_FAIL;
	/********************************************** Player Coin UI END **********************************************/

	tUiDesc.fSizeX = 256.f;
	tUiDesc.fSizeY = 73.f * 0.8f;
	tUiDesc.fX = 0.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 500.f;
	tUiDesc.fZ = 0.f;
	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_Player_Restart", L"Prototype_GameObject_UI_Player_Restart", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_UI_NextStage", L"Prototype_GameObject_UI_Etc_NextStage", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_UI_OpenBox", L"Prototype_GameObject_UI_Etc_OpenBox", &tUiDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_UI_Player(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, L"Prototype_GameObject_UI_PlayerIcon")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, L"Prototype_GameObject_UI_PlayerIcon_Bg")))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Player_UI_Damaged", L"Prototype_GameObject_Player_Damaged")))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_UI_Player_Hp_Bar(const wstring& strLayerTag)
{
	CUI::UIDESC tUiDesc;

	tUiDesc.fSizeX = 150.f;
	tUiDesc.fSizeY = 8.f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 253.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 653.f;
	tUiDesc.fZ = 0.3f;
	//tUiDesc.fRot = 2.f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Character_HPBar3";

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, L"Prototype_GameObject_Player_HP_Bar")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_UI_Player_Shield_Bar(const wstring& strLayerTag)
{
	CUI::UIDESC tUiDesc;

	tUiDesc.fSizeX = 150.f;
	tUiDesc.fSizeY = 8.f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 258.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 639.f;
	tUiDesc.fZ = 0.3f;
	//tUiDesc.fRot = 2.f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Character_HPBar3";

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, L"Prototype_GameObject_Player_Shield_Bar")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_UI_MiniMap_Bg(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, L"Prototype_GameObject_UI_MiniMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, strLayerTag, L"Prototype_GameObject_UI_MiniMap_Player")))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_UI_White_Num(const wstring& strLayerTag)
{
	CUI_TextNum::NUM_DESC tNumDesc;
	tNumDesc.fSizeX = 20.f;
	tNumDesc.fSizeY = 20.f;
	tNumDesc.fX = -(g_iWinSizeX * 0.5f) + 1100.f;
	tNumDesc.fY = (g_iWinSizeY * 0.5f) - 570.f;
	tNumDesc.eTextureType = CUI_TextNum::NUM_L;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_WeaponNum_Mag", L"Prototype_GameObject_UI_Weapon_CountNum", &tNumDesc)))
		return E_FAIL;

	tNumDesc.fSizeX = 20.f;
	tNumDesc.fSizeY = 20.f;
	tNumDesc.fX = -(g_iWinSizeX * 0.5f) + 1200.f;
	tNumDesc.fY = (g_iWinSizeY * 0.5f) - 570.f;
	tNumDesc.eTextureType = CUI_TextNum::NUM_R;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_WeaponNum_Tot", L"Prototype_GameObject_UI_Weapon_CountNum", &tNumDesc)))
		return E_FAIL;

	tNumDesc.fSizeX = 20.f;
	tNumDesc.fSizeY = 20.f;
	tNumDesc.fX = -(g_iWinSizeX * 0.5f) + 1120.f;
	tNumDesc.fY = (g_iWinSizeY * 0.5f) - 570.f;
	tNumDesc.eTextureType = CUI_TextNum::DASH;

	if (FAILED(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_UI_WeaponNum_Dash", L"Prototype_GameObject_UI_Weapon_CountNum", &tNumDesc)))
		return E_FAIL;

	return S_OK;

	//_wstring sNumText = L"1234567890";
	//
	//for (int i = 0; i < sNumText.length(); ++i)
	//{
	//	tNumDesc.fX += 20.f;
	//	tNumDesc.num = sNumText[i];
	//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, L"Prototype_GameObject_UI_White_Num", &tNumDesc)))
	//		return E_FAIL;
	//}
	//
	//return S_OK;
}

CLevel_Stage2 * CLevel_Stage2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage2*	pInstance = new CLevel_Stage2(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"Failed to Created : CLevel_Stage2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage2::Free()
{
	__super::Free();

}
