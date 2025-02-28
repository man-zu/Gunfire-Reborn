#include "Loader.h"
#include "GameInstance.h"
#include "Client_Header.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

// typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);
unsigned int APIENTRY Loading_Main(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;	

	return pLoader->Loading();	
}


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

unsigned int CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	/* COM 라이브러리를 사용하는 각 스레드에 대해 한 번 이상 호출되어야 하며 일반적으로 한 번만 호출 */
	CoInitializeEx(nullptr, 0);

	HRESULT		hr{};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo_Level();
		break;
	case LEVEL_TOOL:
		hr = Loading_For_GamePlay_Level();
		break;
	case LEVEL_STAGE1:
		hr = Loading_For_Stage1_Level();
		break;
	case LEVEL_STAGE2:
		hr = Loading_For_Stage2_Level();
		break;
	case LEVEL_STAGE1_BOSS:
		hr = Loading_For_Stage1_Boss_Level();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return 1;

	return 0;
}

HRESULT CLoader::Loading_For_Logo_Level()
{	
	/***************** Make Binary **************/

	//Resources/Models/player/Skill/E
	//C:\jusin\My3D\Client\Bin\Resources\Models\Monster\Boss_JumpStone
	//CLoadModel::MakeData(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/map/Door2/", "../Bin/Resources/Binary/Map/STAGE1/");

	/***************** Make Binary **************/

	lstrcpy(m_szLoadingText, L"텍스쳐를 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_BackGround",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/background/bg_login.png"))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, L"모델을(를) 로딩 중 입니다.");


	lstrcpy(m_szLoadingText, L"사운드을(를) 로딩 중 입니다.");
	

	lstrcpy(m_szLoadingText, L"셰이더을(를) 로딩 중 입니다.");
	

	lstrcpy(m_szLoadingText, L"객체원형을(를) 로딩 중 입니다.");

	lstrcpy(m_szLoadingText, L"로딩이 완료되었습니다.");

	m_isFinished = true;
	
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{

	/***************** TEST CODE **************/

	/* 확인하고 싶은 모델 여기서 확인 */
	///* For.Prototype_Component_Model_Map_Stage1 */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1, TEXT("Prototype_Component_Model_Map_Stage1"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/map/STAGE1/area1.fbx", tmp))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Map_Stage1 */
	//if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Map_Stage1",
	//	CMap_Stage1_Boss::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/***************** TEST CODE **************/









	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"텍스쳐를 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_Terrain */ 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, L"Prototype_Component_Texture_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Terrain/Tile0.dds", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BackGround_loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_BackGround_loading",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/background/loading/bg_loading%d.png", 12))))
		return E_FAIL;

	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"셰이더을(를) 로딩 중 입니다.");
	lstrcpy(m_szLoadingText, TEXT("콜라이더을(를) 로딩 중 입니다."));
	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩 중 입니다."));

	/**************************************************************************************************/
	
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/ToolSavedFiles/Navigation/Navigation.dat")))))
		return E_FAIL;
	
	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"모델을(를) 로딩 중 입니다.");



	_matrix		LocalTransformMatrix = XMMatrixIdentity();


	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"사운드을(를) 로딩 중 입니다.");

	lstrcpy(m_szLoadingText, L"객체 원형을(를) 로딩 중 입니다.");
	
	/**************************************************************************************************/
	/*****************************************	UI ****************************************/
	lstrcpy(m_szLoadingText, L"UI을(를) 로딩 중 입니다.");

	/**************************************************************************************/
	/**************************************************************************************/

	lstrcpy(m_szLoadingText, L"로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1_Level()
{

	lstrcpy(m_szLoadingText, L"텍스쳐를 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1, L"Prototype_Component_Texture_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Terrain/Tile0.png", 1))))
		return E_FAIL;

	/**************************************************************************************************/
	
	Loading_For_Static();

	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩 중 입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1, TEXT("Prototype_Component_Navigation_Stage1"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/ToolSavedFiles/Navigation/Stage1.dat")))))
		return E_FAIL;

	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"모델을(를) 로딩 중 입니다.");

	_matrix LocalTransformMatrix = XMMatrixIdentity();

	///* For.Prototype_Component_Model_arrow */
	//LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_arrow",
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/arrow/arrow.fbx", LocalTransformMatrix))))
	//	return E_FAIL;

	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"사운드을(를) 로딩 중 입니다.");


	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"객체원형을(를) 로딩 중 입니다.");

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player",
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	/* For.Prototype_GameObject_Map_Stage1 */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Map_Stage1",
		CMap_Stage1::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	/**************************************************************************************/
	/* Object */
	Load_Saved_Files(LEVEL_STAGE1, "stage1.json");
	
	_matrix		tmp = XMMatrixIdentity();
	tmp = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* 확인하고 싶은 모델 여기서 확인 */
	/* For.Prototype_Component_Model_Map_Stage1 */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Model_Map_Stage1"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/map/BOSS/BossStage2.fbx", tmp))))
	//	return E_FAIL;

	/**************************************************************************************/

	lstrcpy(m_szLoadingText, L"로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage2_Level()
{
	lstrcpy(m_szLoadingText, L"텍스쳐를 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE2, L"Prototype_Component_Texture_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Terrain/Tile0.png", 1))))
		return E_FAIL;

	/**************************************************************************************************/

	Loading_For_Static();

	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩 중 입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE2, TEXT("Prototype_Component_Navigation_Stage2"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/ToolSavedFiles/Navigation/Stage2.dat")))))
		//CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/ToolSavedFiles/Navigation/STAGE2.dat")))))
		return E_FAIL;
	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"모델을(를) 로딩 중 입니다.");

	_matrix LocalTransformMatrix = XMMatrixIdentity();

	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"사운드을(를) 로딩 중 입니다.");


	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"객체원형을(를) 로딩 중 입니다.");


	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	for (int i = 0; i < 99999999; ++i)
	{
		int a = 1;
	}

	/**************************************************************************************/
	/* Object */
	Load_Saved_Files(LEVEL_STAGE2, "stage2.json");

	_matrix		tmp = XMMatrixIdentity();
	tmp = XMMatrixRotationY(XMConvertToRadians(180.0f));

	///* 확인하고 싶은 모델 여기서 확인 */
	///* For.Prototype_Component_Model_Map_STAGE2 */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE2, TEXT("Prototype_Component_Model_test"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/map/BOSS/BossStage2.fbx", tmp))))
	//	return E_FAIL;

	/**************************************************************************************/

	lstrcpy(m_szLoadingText, L"로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1_Boss_Level()
{

	_matrix		tmp = XMMatrixIdentity();
	tmp = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* 확인하고 싶은 모델 여기서 확인 */
	/* For.Prototype_Component_Model_Map_Stage1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Model_Map_Stage1_Boss"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/map/BOSS/BossStage.fbx", tmp))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map_Stage1_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Map_Stage1_Boss",
		CMap_Stage1_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩 중 입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Navigation_StageBoss"),
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/ToolSavedFiles/Navigation/Stage1_boss.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, L"텍스쳐을(를) 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1_BOSS, L"Prototype_Component_Texture_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Terrain/Tile0.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Boss_UI_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Boss_UI_Icon",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Monster/BOSS/king2.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Boss_UI_Hp_Bg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Boss_UI_Hp_Bg",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Monster/BOSS/ui/img_boss_hpbg.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Boss_Effect_TargetRing */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Boss_Effect_TargetRing",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/ring/ring_%d.png", 17))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Boss_Effect_Earth */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Boss_Effect_Earth",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/floorCrack/earth_test.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Boss_Effect_GlowCp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Boss_Effect_GlowCp",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/floorCrack/glow_cp.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;

	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"모델을(를) 로딩 중 입니다.");

	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Monster_Boss_65 */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-45.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1_BOSS, L"Prototype_Component_Monster_Boss_65",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/3901.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Monster_Boss_Skill_JumpStone */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-45.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1_BOSS, L"Prototype_Component_Monster_Boss_Skill_JumpStone",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/Effect/Boss_JumpStone/Boss_JumpStone.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Monster_Boss_Skill_JumpStone */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-45.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1_BOSS, L"Prototype_Component_Monster_Boss_Effect_FloorCrash",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/Effect/Boss_JumpFloor/1420_hitstatic_03.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/**************************************************************************************************/
	lstrcpy(m_szLoadingText, L"객체원형을(를) 로딩 중 입니다.");

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Boss_Effect_TargetRing",
		CBoss_Effect_TargetRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Boss_Effect_CrackLight",
		CBoss_Effect_CrackLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Boss_Effect_Crack",
		CBoss_Effect_Crack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Boss",
		CBoss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss_65 */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Boss_65",
		CBoss_65_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss_Skill_JumpStone */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Boss_Skill_JumpStone",
		CBoss_Skill_JumpStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss_Skill_ThrowStone */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Boss_Skill_ThrowStone",
		CBoss_Skill_ThrowStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss_Skill_Shot */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Boss_Skill_Shot",
		CBoss_Skill_Shot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss65HPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Boss65HPBar",
		CUI_Boss65HPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss65ShieldBar */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Boss65ShieldBar",
		CUI_Boss65ShieldBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/**************************************************************************************/

	/**************************************************************************************/

	Loading_For_Static();

	/**************************************************************************************/

	Load_Binary_Files(LEVEL_STAGE1_BOSS, "Map/BOSS/");
	
	//Load_Saved_Files(LEVEL_STAGE1, "stage1.json");

	/**************************************************************************************/


	lstrcpy(m_szLoadingText, L"로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Player_UI()
{
	lstrcpy(m_szLoadingText, L"[static] UI을(를) 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_loop_ring */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_loop_ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/effect/particle/skill_loop_ring_%d.png"), 4))))
		return E_FAIL;

	/* [ Texture ] */
	/* Effect texture*/
	/* For.Prototype_Component_Texture_Effect_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Effect_Glow",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/glow_cp/glow_cp_%d.png", 8))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PlayerWeapon_shot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_PlayerWeapon_shot",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/1409_ray_01.png"))))
		return E_FAIL;

	/* Object texture*/
	/* For.Prototype_Component_Texture_Effect_blueShot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Effect_blueShot",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/blueShot/blueShot_%d.png", 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PlayerDamaged */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_PlayerDamaged",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Player/Effect/damaged.png"))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_PlayerIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_PlayerIcon",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Player/icon_hero2_1.png"))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_PlayerIcon_Bg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_PlayerIcon_Bg",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Player/UI/img_bg1.png"))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Character_HPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Character_HPBar1",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Player/UI/Bar_Character_HPBar_1.png"))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Character_HPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Character_HPBar2",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Player/UI/Bar_Character_HPBar_2.png"))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Character_HPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Character_HPBar3",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Player/UI/Bar_Character_HPBar_3.png"))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Character_SFBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Character_SFBar",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Player/UI/Bar_Character_SFBar.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Minimap_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Minimap_Player",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/MiniMap/icon_own.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Minimap_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Minimap_Monster",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/MiniMap/icon_new_piont.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Minimap_Bg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Minimap_Bg",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/MiniMap/bg_minimap.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_White_Num */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_White_Num",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Text/num/white/%d.png", 12))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Weapon_Num */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Weapon_Num",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Skill/img_shine%d.png", 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Weapon_Skill_bg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Weapon_Skill_bg0",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Skill/img_skill_bg0.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Weapon_Skill_bg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Weapon_Skill_bg1",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Skill/img_skill_bg1.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Weapon_bg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Weapon_bg",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Skill/img_weaponbg.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Weapon_Skill */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Weapon_Skill",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Skill/icon_Skill_%d.png", 3))))
		return E_FAIL;

	/* weapon Img */
	/* For.Prototype_Component_Texture_UI_Weapon_Img */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_UI_Weapon_Img",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Weapon/weapon_%d.png", 4))))
		return E_FAIL;

	/* Effect */
	/* For.Prototype_Component_Texture_Player_Dash_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Player_Dash_Effect",
		//CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Player/Effect/img_ani_panel_%d.png", 4))))
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Player/Effect/Speed_Effect%d.png", 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Player_Damaged */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Player_Damaged",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Player/Effect/damaged.png"))))
		return E_FAIL;

	/* Bullet Effect Texture */
	/* For.Prototype_Component_Texture_Player_Buller_Basic_L */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Colling",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/colling/colling_01_%d.png", 8))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_Buller_Basic_L */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Player_Bullet_Basic_L",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/fire_31/fire_31_%d.png", 12))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BossBullet */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_BossBullet",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/ember_yellow.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect_Fire_01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Effect_Fire_01",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/fire_01/fire_01_%d.png", 10))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect_Fire_02 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Effect_Fire_02",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/fire_02/fire_02_%d.png", 16))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect_gray */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Effect_gray",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/gray/gray_%d.png", 16))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect_DamagedFlash */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Effect_DamagedFlash",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/DamagedFlash/DamagedFlash_%d.png", 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_Buller_Basic_L */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Player_Bullet_Pistol",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/bullet_red.png", 16))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_RestartBg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Player_RestartBg",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/backImg/bg_tips.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_bg_box */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Player_bg_box",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/backImg/bg_box.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_bg_boxItemBg */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Player_bg_boxItemBg",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/backImg/bg_boxItem.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_bg_boxItem */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Player_bg_boxItem",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/BoxItem/Item_%d.png", 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Victory */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Victory",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Text/Victory.png"))))
		return E_FAIL;


	/* [ Prototype ] */

	/* for.Prototype_GameObject_PlayerIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_PlayerIcon",
		CUI_PlayerIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_Player_HP_Bar */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_HP_Bar",
		CUI_PlayerHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_Player_Shield_Bar */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_Shield_Bar",
		CUI_PlayerShieldBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_PlayerIcon_Bg */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_PlayerIcon_Bg",
		CUI_IconBg::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_MiniMap_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MiniMap_Player",
		CUI_MiniMap_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_MiniMap_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MiniMap_Monster",
		CUI_MiniMap_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_PlayerIcon_Bg */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MiniMap",
		CUI_MiniMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_White_Num */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_White_Num",
		CUI_Com_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_PlayerWeapon_bg */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_PlayerWeapon_Img",
		CUI_PlayerWeapon_Img::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_PlayerWeapon_SkillbgR */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_PlayerWeapon_SkillbgR",
		CUI_PlayerWeapon_SkillbgR::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_PlayerWeapon_SkillbgMid */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_PlayerWeapon_SkillbgMid",
		CUI_PlayerWeapon_SkillbgMid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_Weapon_CountNum */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Weapon_CountNum",
		CUI_TextNum::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_CoinCnt */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_CoinCnt",
		CUI_CoinCnt::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_Player_Restart */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Player_Restart",
		CUI_Player_Restart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_Player_Effect_Flame */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_Effect_Flame",
		CPlayer_Effect_Flame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_Player_Effect_Dash */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_Effect_Dash",
		CPlayer_Effect_Dash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_Player_Damaged */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_Damaged",
		CUI_PlayerDamaged::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_Etc_NextStage */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Etc_NextStage",
		CUI_Etc_NextStage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_Etc_OpenBox */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Etc_OpenBox",
		CUI_Etc_OpenBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_Etc_BoxItem */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Etc_BoxItem",
		CUI_Etc_BoxItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_Etc_BoxItemsBg */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Etc_BoxItemsBg",
		CUI_Etc_BoxItemsBg::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_Etc_BoxItems */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Etc_BoxItems",
		CUI_Etc_BoxItems::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_Victory */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Victory",
		CUI_Victory::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading_For_Static_Etc()
{

	lstrcpy(m_szLoadingText, L"[static] Effect 을(를) 로딩 중 입니다.");
	/* For.Prototype_Component_Effect_Common */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Component_Effect_Common",
		CEffect_Common::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, L"[static] Etc 을(를) 로딩 중 입니다.");

	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	/**************************************************************************************/
	/* 텍스쳐 */
	/* For.Prototype_Component_Texture_MonsterHP_BG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_portal",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/portal.png"))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_MonsterHP_BG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_MonsterHP_BG",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/UI/bg_monsterHP_normal.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PropCoin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_PropCoin",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/UI/icon_PropCoin.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_textBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_textBG",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/backImg/textBG.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GW_fire */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_GW_fire",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/effect/boss_bg_effect/GW_fire_%d.png", 16))))
		return E_FAIL;

	/**************************************************************************************/

	/* 몬스터 모델 */
	/* For.Prototype_Component_Model_boom */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_boom",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/boom/boom.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Box */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Box",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Object/Box/Box.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_arrow */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_arrow",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/arrow/arrow.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_arrow */
	LocalTransformMatrix = XMMatrixScaling(0.04f, 0.04f, 0.04f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Weapon_Arrow",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/arrow/weapon_Arrow.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_sword */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_sword",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/sword/sword.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_shieldMan */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_shieldMan",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/shieldMan/shieldMan.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* 플레이어 모델 */
	/* For.Prototype_Component_Model_PlayerBasic *///1410_loop_posion
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-55.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Player_Skill",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/player/Skill/101.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_PlayerBasic */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(170.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Player_Pistol",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/player/weapon/Pistol/1202_Right.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_PlayerBasic */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(170.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Player_SR",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/player/weapon/SR/1501_Right.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_PlayerBasic */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(160.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Player_Basic",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/player/weapon/basic/1107_Right.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Player_BlueEye */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(170.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Player_BlueEye",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/player/weapon/blueEye/1302_Right.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Monster_Boss_65 */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_PlayerBody",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/player/body/ATTACK/101_Temp.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_PropCoin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_PropCoin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Object/Coin/PropCoin.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Door */
	LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Door",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/map/Door/Door.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_MiniDoor */
	LocalTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_MiniDoor_top",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/map/Door2/door_top.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_MiniDoor */
	LocalTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_MiniDoor_bottom",
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/map/Door2/door_bottom.fbx", LocalTransformMatrix))))
		return E_FAIL;

	/**************************************************************************************/
	/* 원형 */
	/* For.Prototype_Component_GameObject_Coin */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Component_GameObject_Monster_Effect",
		CMonster_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_GameObject_Coin */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Component_GameObject_Coin",
		CCoin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Terrain",
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Terrain/Height.bmp", 200, 200))))
		return E_FAIL;

	/* For.Prototype_Component_Navigation_Tool */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Tool"),
		CNavigation::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Main */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Camera_Free",
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Terrain",
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 무기 */
	/* For.Prototype_GameObject_Player_Basic */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_Basic",
		CPlayer_Basic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Basic */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_BlueEye",
		CPlayer_BlueEye::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Basic */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_SR",
		CPlayer_SR::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Basic */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_Pistol",
		CPlayer_Pistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Basic */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_Skill",
		CPlayer_Skill::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Player */
	/* For.Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Body_Player",
		CPlayer_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_WeaponMgr",
		CPlayer_WeaponMgr::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Camera_Player",
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Monster_Boom */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_Boom",
		CMonster_Boom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_BoomPart */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_BoomPart",
		CMonster_BoomPart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_Arrow",
		CMonster_Arrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_Arrow_bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_Arrow_bullet",
		CArrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_Sword",
		CMonster_Sword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_ShieldMan */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_ShieldMan",
		CMonster_ShieldMan::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_ShieldMan */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_ShieldManPart",
		CMonster_ShieldManPart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_Bullet",
		CPlayer_Bullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_BlueBullet */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Player_BlueBullet",
		CPlayer_BlueBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Door */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Door",
		CDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MiniDoor */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MiniDoor",
		CMiniDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Box */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Box",
		CBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Box */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_BoxPart",
		CBoxPart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PlayerPosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Obj_PosionBall",
		CLoadModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Binary/Player/posionBall.data", LocalTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_PlayerPosion",
		CPlayer_posion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_NextPortal",
		CNextPortal::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/**************************************************************************************/
	/* Effect */
	/* For.Prototype_GameObject_Particle_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Explosion"),
		CParticle_Explosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle_BoxOpen */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_BoxOpen"),
		CParticle_BoxOpen::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle_BossLevel */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_BossLevel"),
		CParticle_BossLevel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_Explosion */
	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};
	InstanceDesc.iNumInstance = 15;
	InstanceDesc.vScale = _float2(0.2f, 0.7f);
	InstanceDesc.vCenter = _float3(0.f, 0.0f, 0.f);
	InstanceDesc.vRange = _float3(7.f, 10.f, 7.f);
	InstanceDesc.vSpeed = _float2(7.f, 8.5f);
	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vLifeTime = _float2(5.f, 5.f);
	InstanceDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_Box */
	InstanceDesc.iNumInstance = 20;
	InstanceDesc.vScale = _float2(0.1f, 0.3f);
	InstanceDesc.vCenter = _float3(0.f, 0.0f, 0.f);
	InstanceDesc.vRange = _float3(2.3f, 2.3f, 1.3f);
	InstanceDesc.vSpeed = _float2(1.8f, 2.2f);
	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vLifeTime = _float2(1.3f, 3.3f);
	InstanceDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Box"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_BossLevel */
	InstanceDesc.iNumInstance = 200;
	InstanceDesc.vScale = _float2(1.1f, 2.2f);
	InstanceDesc.vCenter = _float3(0.f, 0.0f, 0.f);
	InstanceDesc.vRange = _float3(300.3f, 100.3f, 300.3f);
	InstanceDesc.vSpeed = _float2(3.f, 4.5f);
	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vLifeTime = _float2(5.f, 5.f);
	InstanceDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_BossLevel"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Sound()
{
	ADD_SOUND(L"maze", L"../Bin/Resources/Sound/bgm/maze.mp3");
	ADD_SOUND(L"Boss", L"../Bin/Resources/Sound/bgm/Boss.mp3");

	/* Player */
	ADD_SOUND(L"heroDash", L"../Bin/Resources/Sound/player/heroDash.mp3");
	ADD_SOUND(L"wpn0", L"../Bin/Resources/Sound/player/wpn0.mp3");
	ADD_SOUND(L"wpn1", L"../Bin/Resources/Sound/player/wpn1.mp3");
	ADD_SOUND(L"wpnNoBullet", L"../Bin/Resources/Sound/player/wpnNoBullet.mp3");
	ADD_SOUND(L"walk", L"../Bin/Resources/Sound/player/walk.mp3");
	ADD_SOUND(L"PlayerDamaged", L"../Bin/Resources/Sound/player/PlayerDamaged.mp3");
	ADD_SOUND(L"PlayerDie", L"../Bin/Resources/Sound/player/PlayerDie.mp3");
	ADD_SOUND(L"PlayerReAlive", L"../Bin/Resources/Sound/player/PlayerReAlive.mp3");
	ADD_SOUND(L"wpnBasicReload", L"../Bin/Resources/Sound/player/wpnBasicReload.mp3");
	ADD_SOUND(L"wpnBasicReload2", L"../Bin/Resources/Sound/player/wpnBasicReload2.mp3");
	ADD_SOUND(L"wpnBlueEyeReload", L"../Bin/Resources/Sound/player/wpnBlueEyeReload.mp3");
	ADD_SOUND(L"wpnBlueEyeReload2", L"../Bin/Resources/Sound/player/wpnBlueEyeReload2.mp3");

	/* Etc */
	ADD_SOUND(L"BoxOpen", L"../Bin/Resources/Sound/BoxOpen.mp3");
	ADD_SOUND(L"MiniDoorOpen", L"../Bin/Resources/Sound/MiniDoorOpen.mp3");
	ADD_SOUND(L"NextStage", L"../Bin/Resources/Sound/NextStage.mp3");
	ADD_SOUND(L"BossDoor", L"../Bin/Resources/Sound/BossDoor.mp3");
	ADD_SOUND(L"getItem", L"../Bin/Resources/Sound/getItem.mp3");

	/* Monster */
	ADD_SOUND(L"ArrowDamaged", L"../Bin/Resources/Sound/monster/ArrowDamaged.mp3");
	ADD_SOUND(L"ArrowDie", L"../Bin/Resources/Sound/monster/ArrowDie.mp3");
	ADD_SOUND(L"ShieldManDamaged", L"../Bin/Resources/Sound/monster/ShieldManDamaged.mp3");
	ADD_SOUND(L"ShieldManDie", L"../Bin/Resources/Sound/monster/ShieldManDie.mp3");
	ADD_SOUND(L"swordDamaged", L"../Bin/Resources/Sound/monster/swordDamaged.mp3");
	ADD_SOUND(L"swordDie", L"../Bin/Resources/Sound/monster/swordDie.mp3");

	/* Boss */
	ADD_SOUND(L"BossIntro", L"../Bin/Resources/Sound/monster/Boss/BossIntro.mp3");
	ADD_SOUND(L"BossArrr1", L"../Bin/Resources/Sound/monster/Boss/BossArrr1.mp3");
	ADD_SOUND(L"BossArrr2", L"../Bin/Resources/Sound/monster/Boss/BossArrr2.mp3");
	ADD_SOUND(L"BossArrr3", L"../Bin/Resources/Sound/monster/Boss/BossArrr3.mp3");
	ADD_SOUND(L"BossArrr4", L"../Bin/Resources/Sound/monster/Boss/BossArrr4.mp3");
	ADD_SOUND(L"BossDie", L"../Bin/Resources/Sound/monster/Boss/BossDie.mp3");
	ADD_SOUND(L"BossDieSound", L"../Bin/Resources/Sound/monster/Boss/BossDieSound.mp3");
	ADD_SOUND(L"ThrowStone", L"../Bin/Resources/Sound/monster/Boss/Rock.mp3");
	ADD_SOUND(L"JumpStone", L"../Bin/Resources/Sound/monster/Boss/RockBoom.mp3");
	ADD_SOUND(L"BossShot", L"../Bin/Resources/Sound/monster/Boss/BossShot.mp3");

	VOLUME(L"wpn1", 10);
	VOLUME(L"ThrowStone", 70);
	VOLUME(L"JumpStone", 80);
	VOLUME(L"BossArrr1", 80);
	VOLUME(L"BossDie", 80);
	VOLUME(L"BossDieSound", 80);

	return S_OK;
}

void CLoader::Loading_For_Static()
{
	if (m_pGameInstance->IsStaticLoaded())
		return;

	Loading_For_Shader();
	Loading_For_Collider();
	Loading_For_Player_UI();
	Loading_For_Static_Etc();

	Loading_For_Sound();

	Load_Binary_Files(LEVEL_STATIC, "Map/STAGE1/");

	m_pGameInstance->StaticLoad();

	return;
}

HRESULT CLoader::Loading_For_Collider()
{
	lstrcpy(m_szLoadingText, TEXT("[static] 콜라이더을(를) 로딩 중 입니다."));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Shader()
{
	lstrcpy(m_szLoadingText, L"[static] 셰이더을(를) 로딩 중 입니다.");

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Shader_VtxMesh",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/Shaderfiles/Shader_VtxMesh.hlsl", VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Shader_VtxNorTex",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxNorTex.hlsl", VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimMesh",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Shader_VtxCube",
		CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxCube.hlsl", VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTexAlpha"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTexAlpha.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;


	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};
	/* For.Prototype_Component_VIBuffer_Particle */
	InstanceDesc.iNumInstance = 1;
	InstanceDesc.vScale = _float2(10.2f, 10.5f);
	InstanceDesc.vCenter = _float3(0.f, .0f, 0.f);
	InstanceDesc.vRange = _float3(129.f, 1.f, 129.f);
	InstanceDesc.vSpeed = _float2(1.f, 1.f);
	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vLifeTime = _float2(20.f, 50.f);
	InstanceDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &InstanceDesc))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Binary_Files(LEVEL eLevel, const string _path)
{
	_matrix		tmp = XMMatrixIdentity();
	tmp = XMMatrixRotationY(XMConvertToRadians(180.0f));
	
	/* 확인하고 싶은 모델 여기서 확인 */
	///* For.Prototype_Component_Model_Map_Stage1 */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STAGE1, TEXT("Prototype_Component_Model_Map_Stage1"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/map/STAGE1/area1.fbx", tmp))))
	//	return E_FAIL;
	
	///* For.Prototype_GameObject_Map_Stage1 */
	//if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Map_Stage1",
	//	CMap_Stage1_Boss::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;


	lstrcpy(m_szLoadingText, L"binary 파일을(를) 로딩 중 입니다.");

	/*****************************************	Object ****************************************/
	_matrix		LocalTransformMatrix = XMMatrixIdentity();

	string path = "../Bin/Resources/Binary/" + _path + "*.*";
	string path2 = "../Bin/Resources/Binary/" + _path;
	
	struct _finddata_t fd;
	intptr_t handle;

	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
	{
		cout << "[ ERROR ] No file in directory ! " << endl;
		cout << "[ ERROR ] File Path : " << path << endl;
		return E_FAIL;
	}

	std::cout << "===================================================" << std::endl;
	std::cout << " 파일 조회 . . . . " << std::endl;
	do
	{
		string fileName = fd.name;
		string filePullPath = path2 + fd.name;

		size_t idx = fileName.find('.');
		fileName = fileName.substr(0, idx);

		wstring tmp;
		tmp.assign(fileName.begin(), fileName.end());


		cout << "fileName : " << fileName.c_str() << endl;

		if (fileName.length() >= 2)
		{
			/* Binary Model */
			LocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
			if (FAILED(m_pGameInstance->Add_Prototype(eLevel, L"Prototype_Component_Obj_"+ tmp,
				CLoadModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filePullPath.c_str(), LocalTransformMatrix))))
				return E_FAIL;

			/* Binary Model's Prototype */
			if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Obj_"+ tmp,
				CObj_Common::Create(m_pDevice, m_pContext))))
				return E_FAIL;

		}


	} while (_findnext(handle, &fd) == 0);
	_findclose(handle);

	std::cout << "===================================================" << std::endl;


	return S_OK;
}

HRESULT CLoader::Load_Saved_Files(LEVEL eLevel, const string _path)
{
	try {
		std::ifstream file("../Bin/ToolSavedFiles/obj/"+ _path, std::ios::in);
		if (!file.is_open())
		{
			std::cout << "Error opening Saved File.\n";
			return E_FAIL;
		}
		json jDataList;
		file >> jDataList;

		file.close();

		CObject::OBJDESC* pDesc = {};

		for (auto& jData : jDataList)
		{
			pDesc = new CObject::OBJDESC;

			CObject::OBJDESC transDesc{};

			pDesc->sFileName = jData["sFileName"];
			pDesc->sLayerTag = jData["sLayerTag"];
			pDesc->sPrototpyeName = jData["sPrototpyeName"];
			pDesc->sComponentName = transDesc.sComponentName = jData["sComponentName"];
			pDesc->fPosX = transDesc.fPosX = jData["fPosX"];
			pDesc->fPosY = transDesc.fPosY = jData["fPosY"];
			pDesc->fPosZ = transDesc.fPosZ = jData["fPosZ"];
			pDesc->fRotY = transDesc.fRotY = jData["fRotY"];

			wstring sLayerTag = L"";
			sLayerTag.assign(pDesc->sLayerTag.begin(), pDesc->sLayerTag.end());
			wstring sPrototypeName = L"";
			sPrototypeName.assign(pDesc->sPrototpyeName.begin(), pDesc->sPrototpyeName.end());

			transDesc.fSpeedPerSec = 10.f;
			transDesc.fRotationPerSec = XMConvertToRadians(90.0f);

			if (FAILED(m_pGameInstance->Add_Clone(eLevel, sLayerTag, sPrototypeName, &transDesc)))
				return E_FAIL;

			Safe_Delete(pDesc);
		}

		return S_OK;
	}
	catch (std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return E_FAIL;
	}
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(L"Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
