#include "UI_Etc_BoxItemsBg.h"

#include "GameInstance.h"
#include "Player.h"

CUI_Etc_BoxItemsBg::CUI_Etc_BoxItemsBg(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Etc_BoxItemsBg::CUI_Etc_BoxItemsBg(const CUI_Etc_BoxItemsBg & rhs)
	: CUI( rhs )
{
}

HRESULT CUI_Etc_BoxItemsBg::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Etc_BoxItemsBg::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	CUI_Etc_BoxItem::BOXITEM_DESC* pParam = static_cast<CUI_Etc_BoxItem::BOXITEM_DESC*>(pArg);

    /* Binding pArg */
    {
        m_tUIDesc.fSizeX = 600.f;
        m_tUIDesc.fSizeY = 600.f;
		m_tUIDesc.fX = pParam->fX;
		m_tUIDesc.fY = pParam->fY;
        m_tUIDesc.fZ = 0.1f;
    }

    m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, m_tUIDesc.fZ, 1.f));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

    return S_OK;
}

void CUI_Etc_BoxItemsBg::PriorityTick(_float fTimeDelta)
{
}

void CUI_Etc_BoxItemsBg::Tick(_float fTimeDelta)
{
}

void CUI_Etc_BoxItemsBg::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Etc_BoxItemsBg::Render()
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

HRESULT CUI_Etc_BoxItemsBg::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_Player_bg_boxItemBg",
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Etc_BoxItemsBg * CUI_Etc_BoxItemsBg::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Etc_BoxItemsBg*		pInstance = new CUI_Etc_BoxItemsBg(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_Etc_BoxItemsBg::Clone(void * pArg)
{
	CUI_Etc_BoxItemsBg*		pInstance = new CUI_Etc_BoxItemsBg(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Etc_BoxItemsBg::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
