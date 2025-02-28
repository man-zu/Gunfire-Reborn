#include "UI_Com_Number.h"

#include "GameInstance.h"
#include "Player.h"
#include "Level.h"

CUI_Com_Number::CUI_Com_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUI{ pDevice, pContext }
{
}

CUI_Com_Number::CUI_Com_Number(const CUI_Com_Number& rhs)
    :CUI( rhs )
{
}

CUI_Com_Number::~CUI_Com_Number()
{
}

HRESULT CUI_Com_Number::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Com_Number::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC			TransformDesc{};
    TransformDesc.fSpeedPerSec = 1.f;
    TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&TransformDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    /* Binding pArg */
    {
        if (pArg == nullptr)
            return E_FAIL;

        m_tUIDesc.fSizeX = static_cast<CUI_TextNum::NUM_DESC*>(pArg)->fSizeX;
        m_tUIDesc.fSizeY = static_cast<CUI_TextNum::NUM_DESC*>(pArg)->fSizeY;
        m_tUIDesc.fX = static_cast<CUI_TextNum::NUM_DESC*>(pArg)->fX;
        m_tUIDesc.fY = static_cast<CUI_TextNum::NUM_DESC*>(pArg)->fY;
        m_iNum = static_cast<CUI_TextNum::NUM_DESC*>(pArg)->sNum[0] - '0';
        m_eTextureType = static_cast<CUI_TextNum::NUM_DESC*>(pArg)->eTextureType;
        m_iDigit = static_cast<CUI_TextNum::NUM_DESC*>(pArg)->iDisit;
    }

    m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, 0.f, 1.f));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

    return S_OK;
}

void CUI_Com_Number::PriorityTick(_float fTimeDelta)
{
}

void CUI_Com_Number::Tick(_float fTimeDelta)
{

}

void CUI_Com_Number::LateTick(_float fTimeDelta)
{
    if (GET_CURLEVEL == LEVEL_LOADING)
        return;

    if (m_eTextureType == CUI_TextNum::DASH)
    {
        m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
        return;
    }

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

    int iNum = m_iDigit;
    if(m_eTextureType == CUI_TextNum::NUM_L)
        iNum = pPlayer->GetCurWeaponBulletCur();
    else if (m_eTextureType == CUI_TextNum::NUM_R)
        iNum = pPlayer->GetCurWeaponBulletTot();
    else if (m_eTextureType == CUI_TextNum::NUM)
        iNum = pPlayer->GetCoinCnt();

    _uint i = 0;
    vector<_uint> iNumVec;

    while (iNum != 0)
    {
        iNumVec.push_back(iNum % 10);
        iNum /= 10;
        i++;
    }

    if (i < m_iDigit + 1)
    {
        if (m_iDigit == 0)
            m_iNum = 0;
        else
            m_iNum = -1;
    }
    else
        m_iNum = iNumVec[m_iDigit];


    m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Com_Number::Render()
{
    if (m_iNum == -1 && m_eTextureType != CUI_TextNum::DASH)
        return S_OK;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (m_eTextureType == CUI_TextNum::DASH)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 10)))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iNum)))
            return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Com_Number::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_White_Num"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CUI_Com_Number* CUI_Com_Number::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Com_Number* pInstance = new CUI_Com_Number(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_Com_Number"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CUI_Com_Number::Clone(void* pArg)
{
    CUI_Com_Number* pInstance = new CUI_Com_Number(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CUI_Com_Number"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Com_Number::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}

