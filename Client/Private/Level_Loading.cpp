
#include "Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Stage1.h"
#include "Level_Stage2.h"
#include "Level_Stage1_Boss.h"
#include "UI.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext } 
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_iLevelID = LEVEL_LOADING;

	m_eNextLevelID = eNextLevelID;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	/* 자원로딩을 위한 로더객체를 생성해준다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;	

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{

	if (true == m_pLoader->isFinished())
	{
		CLevel* pLevel = { nullptr };
		
		/* LOGO LEVEL SKIP */
		if (m_eNextLevelID == LEVEL_LOGO)
		{
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
		}

		switch (m_eNextLevelID)
		{
		case LEVEL_TOOL:
			pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_STAGE1:
			pLevel = CLevel_Stage1::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_STAGE2:
			pLevel = CLevel_Stage2::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_STAGE1_BOSS:
			pLevel = CLevel_Stage1_Boss::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;

		if (FAILED(m_pGameInstance->Change_Level(pLevel)))
			return;
	}	
}

HRESULT CLevel_Loading::Render()
{	
	m_pLoader->Show_LoadingText();
	
	return S_OK;
}


CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	CUI::UIDESC UIDesc;
	UIDesc.eUIObjType = CUI::UI_LOADING;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, L"Prototype_GameObject_BackGround", &UIDesc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
