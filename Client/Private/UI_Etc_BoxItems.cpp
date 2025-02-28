#include "UI_Etc_BoxItems.h"

#include "GameInstance.h"
#include "Player.h"
#include "UI_Crosshair.h"

CUI_Etc_BoxItems::CUI_Etc_BoxItems(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Etc_BoxItems::CUI_Etc_BoxItems(const CUI_Etc_BoxItems & rhs)
	: CUI( rhs )
{
}

HRESULT CUI_Etc_BoxItems::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Etc_BoxItems::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	CUI_Etc_BoxItem::BOXITEM_DESC* pParam = static_cast<CUI_Etc_BoxItem::BOXITEM_DESC*>(pArg);
	m_bDesc.iIdx = pParam->iIdx;
	m_bDesc.sItemName = pParam->sItemName;
	m_bDesc.sItemInfo = pParam->sItemInfo;
	m_bDesc.fMargin = pParam->fMargin;

    /* Binding pArg */
    {
        m_tUIDesc.fSizeX = 120.f;
        m_tUIDesc.fSizeY = 120.f;

        m_tUIDesc.fX = pParam->fX;
        m_tUIDesc.fY = pParam->fY;
        m_tUIDesc.fZ = 0.1f;
    }

    m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, m_tUIDesc.fZ, 1.f));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


	SetRect(&m_tButtonRect,
		320 + m_bDesc.fMargin,
		200,
		600 + m_bDesc.fMargin,
		520);

    return S_OK;
}

void CUI_Etc_BoxItems::PriorityTick(_float fTimeDelta)
{


}

void CUI_Etc_BoxItems::Tick(_float fTimeDelta)
{
	POINT			ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (TRUE == PtInRect(&m_tButtonRect, ptMouse))
	{
		if (MOUSE_PUSH(DIMK_LBUTTON))
		{
			ItemSelected();

			dynamic_cast<CUI_Etc_BoxItem*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_BoxItemBg")->front())->SetShowYN(false);
			dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front())->MoveStop(false);
			dynamic_cast<CUI_Crosshair*>(GET_LIST(LEVEL_STATIC, L"Layer_Crosshair")->front())->SetFree(false);

			for (list<class CGameObject*>::iterator iter = GET_LIST(LEVEL_STATIC, L"Layer_UI_BoxItemList")->begin();
				iter != GET_LIST(LEVEL_STATIC, L"Layer_UI_BoxItemList")->end(); ++iter)
			{
				(*iter)->Set_Dead(true);
			}
		}
	}
}

void CUI_Etc_BoxItems::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Etc_BoxItems::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_bDesc.iIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGameInstance->WriteText(TEXT("Font_Default"), m_bDesc.sItemName, _float2(415.f+ m_bDesc.fMargin, 220.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, 1.1f);
	m_pGameInstance->WriteText(TEXT("Font_Default"), m_bDesc.sItemInfo, _float2(370.f+ m_bDesc.fMargin, 430.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, 1.1f);
	
	return S_OK;
}

HRESULT CUI_Etc_BoxItems::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_Player_bg_boxItem",
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

void CUI_Etc_BoxItems::ItemSelected()
{
	switch (m_bDesc.iIdx)
	{
	case 0:
		g_bCoinAuto = true;
		break;
	case 1:
		// 체력증가
		MSG_BOX(L"1111");
		dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());
		break;
	case 2:
		// 쉴드 증가
		MSG_BOX(L"2222");
		dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());
		break;
	case 3:
		// 공격력 증가
		MSG_BOX(L"3333");
		dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());
		break;
	default:
		break;
	}
}

CUI_Etc_BoxItems * CUI_Etc_BoxItems::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Etc_BoxItems*		pInstance = new CUI_Etc_BoxItems(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_Etc_BoxItems::Clone(void * pArg)
{
	CUI_Etc_BoxItems*		pInstance = new CUI_Etc_BoxItems(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Etc_BoxItems::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
