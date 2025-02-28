#include "UI_CoinCnt.h"

#include "GameInstance.h"
#include "UI_Com_Number.h"
#include "UI_TextNum.h"
#include "Player.h"

static _uint	m_iCoin = { 0 };
CUI_CoinCnt::CUI_CoinCnt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUI{ pDevice, pContext }
{
}

CUI_CoinCnt::CUI_CoinCnt(const CUI_CoinCnt& rhs)
    :CUI( rhs )
{
}

CUI_CoinCnt::~CUI_CoinCnt()
{
}

HRESULT CUI_CoinCnt::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_CoinCnt::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC			TransformDesc{};
    TransformDesc.fSpeedPerSec = 1.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    m_tUIDesc.fSizeX = 24.f;
    m_tUIDesc.fSizeY = 24.f;
    m_tUIDesc.fX = -(g_iWinSizeX * 0.5f) + 1180.f;
    m_tUIDesc.fY = (g_iWinSizeY * 0.5f) - 50.f;
    m_iNum = 0;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pPlayer = static_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

    return S_OK;
}

void CUI_CoinCnt::PriorityTick(_float fTimeDelta)
{
}

void CUI_CoinCnt::Tick(_float fTimeDelta)
{

}

void CUI_CoinCnt::LateTick(_float fTimeDelta)
{
}

HRESULT CUI_CoinCnt::Render()
{    
    return S_OK;
}

HRESULT CUI_CoinCnt::Ready_Components()
{
    CUI_TextNum::NUM_DESC tNumDesc;
    tNumDesc.fSizeX = m_tUIDesc.fSizeX;
    tNumDesc.fSizeY = m_tUIDesc.fSizeY;
    tNumDesc.fX = m_tUIDesc.fX;
    tNumDesc.fY = m_tUIDesc.fY;
    tNumDesc.eTextureType = CUI_TextNum::NUM;

    _wstring sNumText = L"0000";

    for (int i = 0; i < sNumText.length(); ++i)
    {
        tNumDesc.fX -= 20.f;
        tNumDesc.sNum = sNumText[i];
        tNumDesc.iDisit = i;
        if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_UI_CoinNum", L"Prototype_GameObject_UI_White_Num", &tNumDesc)))
            return E_FAIL;
    }
    return S_OK;
}

CUI_CoinCnt* CUI_CoinCnt::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_CoinCnt* pInstance = new CUI_CoinCnt(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_CoinCnt"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CUI_CoinCnt::Clone(void* pArg)
{
    CUI_CoinCnt* pInstance = new CUI_CoinCnt(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CUI_CoinCnt"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_CoinCnt::Free()
{
    __super::Free();
}

