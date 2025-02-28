#include "UI_TextNum.h"

#include "GameInstance.h"
#include "UI_Com_Number.h"

CUI_TextNum::CUI_TextNum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUI{ pDevice, pContext }
{
}

CUI_TextNum::CUI_TextNum(const CUI_TextNum& rhs)
    :CUI( rhs )
{
}

CUI_TextNum::~CUI_TextNum()
{
}

HRESULT CUI_TextNum::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_TextNum::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC			TransformDesc{};
    TransformDesc.fSpeedPerSec = 1.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;


    /* Binding pArg */
    {
        if (pArg == nullptr)
            return E_FAIL;

        m_tUIDesc.fSizeX = static_cast<NUM_DESC*>(pArg)->fSizeX;
        m_tUIDesc.fSizeY = static_cast<NUM_DESC*>(pArg)->fSizeY;
        m_tUIDesc.fX = static_cast<NUM_DESC*>(pArg)->fX;
        m_tUIDesc.fY = static_cast<NUM_DESC*>(pArg)->fY;
        m_iNum = static_cast<NUM_DESC*>(pArg)->sNum[0] - '0';
        m_eTextureType = static_cast<NUM_DESC*>(pArg)->eTextureType;
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_TextNum::PriorityTick(_float fTimeDelta)
{
}

void CUI_TextNum::Tick(_float fTimeDelta)
{

}

void CUI_TextNum::LateTick(_float fTimeDelta)
{
}

HRESULT CUI_TextNum::Render()
{    
    return S_OK;
}

HRESULT CUI_TextNum::Ready_Components()
{
    NUM_DESC tNumDesc;
    tNumDesc.fSizeX = m_tUIDesc.fSizeX;
    tNumDesc.fSizeY = m_tUIDesc.fSizeY;
    tNumDesc.fX = m_tUIDesc.fX;
    tNumDesc.fY = m_tUIDesc.fY;
    tNumDesc.eTextureType = m_eTextureType;

    _wstring sNumText = tNumDesc.eTextureType == DASH? L"0" : L"0000";

    for (int i = 0; i < sNumText.length(); ++i)
    {
        tNumDesc.fX -= 20.f;
        tNumDesc.sNum = sNumText[i];
        tNumDesc.iDisit = i;
        if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_UI_White_Num_Left", L"Prototype_GameObject_UI_White_Num", &tNumDesc)))
            return E_FAIL;
    }
    return S_OK;
}

CUI_TextNum* CUI_TextNum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_TextNum* pInstance = new CUI_TextNum(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_TextNum"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CUI_TextNum::Clone(void* pArg)
{
    CUI_TextNum* pInstance = new CUI_TextNum(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CUI_TextNum"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_TextNum::Free()
{
    __super::Free();
}

