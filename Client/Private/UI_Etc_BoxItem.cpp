#include "UI_Etc_BoxItem.h"

#include "GameInstance.h"
#include "Player.h"

CUI_Etc_BoxItem::CUI_Etc_BoxItem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Etc_BoxItem::CUI_Etc_BoxItem(const CUI_Etc_BoxItem & rhs)
	: CUI( rhs )
{
}

HRESULT CUI_Etc_BoxItem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Etc_BoxItem::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	if(FAILED(Ready_Items()))
		return E_FAIL;

    /* Binding pArg */
    {
        if (pArg == nullptr)
            return E_FAIL;

        m_tUIDesc.fSizeX = 1280.f;
        m_tUIDesc.fSizeY = 720.f;
        m_tUIDesc.fX = 0.f;
        m_tUIDesc.fY = 0.f;
        m_tUIDesc.fZ = 0.2f;
    }

    m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, m_tUIDesc.fZ, 1.f));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

    return S_OK;
}

void CUI_Etc_BoxItem::PriorityTick(_float fTimeDelta)
{
}

void CUI_Etc_BoxItem::Tick(_float fTimeDelta)
{
	if (m_bShow && !m_bCreateItem)
	{
		m_bCreateItem = true;

		for (int i = 0; i < 2; ++i)
		{
			m_bItemVec[i].fX = -180.f + 380.f * i;
			m_bItemVec[i].fY = 0.f;
			m_bItemVec[i].fMargin = 380.f * i;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_UI_BoxItemList", L"Prototype_GameObject_UI_Etc_BoxItemsBg", &m_bItemVec[i])))
				MSG_BOX(L"error");

			m_bItemVec[i].fY = 45.f;
			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_UI_BoxItemList", L"Prototype_GameObject_UI_Etc_BoxItems", &m_bItemVec[i])))
				MSG_BOX(L"error");
		}
	}
}

void CUI_Etc_BoxItem::LateTick(_float fTimeDelta)
{
	if(m_bShow)
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Etc_BoxItem::Render()
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

HRESULT CUI_Etc_BoxItem::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_Player_bg_box",
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Etc_BoxItem::Ready_Items()
{
	BOXITEM_DESC desc;
	desc.iIdx = 0;
	desc.sItemName = L"아이템 줍기";
	desc.sItemInfo = L"일정거리 내 아이템 자동 줍기";
	m_bItemVec.push_back(desc);

	desc.iIdx = 1;
	desc.sItemName = L"체력   증가";
	desc.sItemInfo = L"플레이어 체력 증가 +50";
	m_bItemVec.push_back(desc);

	desc.iIdx = 2;
	desc.sItemName = L"쉴드   증가";
	desc.sItemInfo = L"플레이어 쉴드 증가 +50";
	m_bItemVec.push_back(desc);

	desc.iIdx = 3;
	desc.sItemName = L"데미지 증가";
	desc.sItemInfo = L"총 데미지 +10%";
	m_bItemVec.push_back(desc);

	return S_OK;
}

CUI_Etc_BoxItem * CUI_Etc_BoxItem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Etc_BoxItem*		pInstance = new CUI_Etc_BoxItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_Etc_BoxItem::Clone(void * pArg)
{
	CUI_Etc_BoxItem*		pInstance = new CUI_Etc_BoxItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Etc_BoxItem::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
