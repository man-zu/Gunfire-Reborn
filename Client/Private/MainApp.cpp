

#include "../Public/MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "GameObject.h"
#include "ImGui_Manager.h"

//
#include "UI_Crosshair.h"
#include "BackGround.h"
#include "UI_Common.h"

CMainApp::CMainApp()
	: m_pGameInstance {CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);

	//D3D11_SAMPLER_DESC
}

HRESULT CMainApp::Initialize()
{

	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(L"Font_Default", L"../Bin/Resources/Fonts/gunfire.spritefont")))
		return E_FAIL;

	if (FAILED(Ready_Prototoype_Component_ForStatic()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;	

#ifdef _DEBUG
	m_pTool = CImGui_Manager::Get_Instance();
	if (nullptr == m_pTool)
		return E_FAIL;

	if (FAILED(m_pTool->Initialize(m_pDevice, m_pContext, g_hWnd)))
		return E_FAIL;

#endif

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);

	m_pTool->Tick();
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.0f, 0.15f, 0.4f, 1.0f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();
	m_pTool->Render();

	m_pGameInstance->Present();


	return S_OK;
}



HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototoype_Component_ForStatic()
{
	ADD_SOUND(L"main", L"../Bin/Resources/Sound/bgm/main.mp3");

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* Texture */
	/* For.Prototype_Component_Texture_BackGround_loading_main */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_BackGround_loading_main",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/background/loading/bg_loading_main.png"))))
		return E_FAIL;
	
	/* For.Prototype_Component_Text_End */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Text_End",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Text/Text_End.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Text_Play */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Text_Play",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Text/Text_Play.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BackGround_loading_main */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Logo_title",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Logo/title.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BackGround_loading_main */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Logo_fire",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Logo/LogoFire/img_logofire_%d.png", 30))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Mouse",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/UI/img_mouse.png"))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Crosshair */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Texture_Crosshair",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/UI/crosshair.png"))))
		return E_FAIL;

	/* °´Ã¼ ¿øÇü */
	/* for.Prototype_GameObject_UI_Crosshar */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Crosshar",
		CUI_Crosshair::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_BackGround",
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for.Prototype_GameObject_UI_Common */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Common",
		CUI_Common::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pTool);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}

