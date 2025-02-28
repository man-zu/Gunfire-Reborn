
#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "UI.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext } 
{

}

HRESULT CLevel_Logo::Initialize()
{
	m_iLevelID = LEVEL_LOGO;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Crosshair(L"Layer_Crosshair")))
		return E_FAIL;

	CUI::UIDESC tUiDesc;
	tUiDesc.fSizeX = 599.f * 0.7f;
	tUiDesc.fSizeY = 191.f * 0.7f;
	tUiDesc.fX = -(g_iWinSizeX * 0.5f) + 830.f;
	tUiDesc.fY = (g_iWinSizeY * 0.5f) - 190.f;
	tUiDesc.sTextureTag = L"Prototype_Component_Texture_Logo_title";
	tUiDesc.iTextureIdx = 1;
	
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LOGO, L"Layer_UI_Title", L"Prototype_GameObject_UI_Common", &tUiDesc)))
		return E_FAIL;

	PLAY(L"main", true);

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1))))
			return;
	}
	else if (KEY_PUSH(DIK_B))
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1_BOSS))))
			return;
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI_Crosshair(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, strLayerTag, L"Prototype_GameObject_UI_Crosshar")))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo*	pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

}
