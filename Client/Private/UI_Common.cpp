#include "UI_Common.h"

#include "GameInstance.h"

CUI_Common::CUI_Common(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Common::CUI_Common(const CUI_Common & rhs)
	: CUI( rhs )
{
}

HRESULT CUI_Common::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Common::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components(static_cast<CUI::UIDESC*>(pArg)->sTextureTag)))
		return E_FAIL;

    /* Binding pArg */
    {
        if (pArg == nullptr)
            return E_FAIL;

        m_tUIDesc.fSizeX = static_cast<CUI::UIDESC*>(pArg)->fSizeX;
        m_tUIDesc.fSizeY = static_cast<CUI::UIDESC*>(pArg)->fSizeY;
        m_tUIDesc.fX = static_cast<CUI::UIDESC*>(pArg)->fX;
        m_tUIDesc.fY = static_cast<CUI::UIDESC*>(pArg)->fY;
        m_tUIDesc.fZ = static_cast<CUI::UIDESC*>(pArg)->fZ;
    }

    m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, m_tUIDesc.fZ, 1.f));

	m_pTransformCom->Rotation({ 0.f, 0.f, 1.f }, XMConvertToRadians(static_cast<CUI::UIDESC*>(pArg)->fRot));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

    return S_OK;
}

void CUI_Common::PriorityTick(_float fTimeDelta)
{
}

void CUI_Common::Tick(_float fTimeDelta)
{
}

void CUI_Common::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Common::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Common::Ready_Components(wstring sTextureTag)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, sTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Common * CUI_Common::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Common*		pInstance = new CUI_Common(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_Common::Clone(void * pArg)
{
	CUI_Common*		pInstance = new CUI_Common(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Common::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
