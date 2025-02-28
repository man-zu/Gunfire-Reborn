#include "UI_Crosshair.h"

#include "GameInstance.h"
#include "Level.h"

_bool CUI_Crosshair::m_bFree = { true };
//_bool CUI_Crosshair::m_bFree = { false };

CUI_Crosshair::CUI_Crosshair(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Crosshair::CUI_Crosshair(const CUI_Crosshair & rhs)
	: CUI( rhs )
{
}

HRESULT CUI_Crosshair::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Crosshair::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_tUIDesc.fSizeX = 50;
	m_tUIDesc.fSizeY = 50;
	m_tUIDesc.fX = 0.f;
	m_tUIDesc.fY = 0.f;

	ShowCursor(false);

	return S_OK;
}

void CUI_Crosshair::PriorityTick(_float fTimeDelta)
{
	if (KEY_PUSH(DIK_F1))
		m_bFree = !m_bFree;

	if (GET_CURLEVEL == LEVEL_LOADING || GET_CURLEVEL == LEVEL_LOGO)
	{
		m_bGameLevel = false;
		m_bFree = true;
	}
	else
	{
		if (!m_bGameLevel)
		{
			m_bGameLevel = true;
			m_bFree = false;
		}
	}

	if (false == m_bFree)
	{
		m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, 0.f, 1.f));

		// 화면 마우스 고정
		RECT rc;
		POINT p1, p2;
		GetClientRect(g_hWnd, &rc);

		p1.x = rc.left / 2;
		p1.y = rc.top / 2;
		p2.x = rc.right / 2;
		p2.y = rc.bottom / 2;

		ClientToScreen(g_hWnd, &p1);
		ClientToScreen(g_hWnd, &p2);

		rc.left = p2.x - 1;
		rc.top = p2.y - 1;
		rc.right = p2.x + 1;
		rc.bottom = p2.y + 1;
		ClipCursor(&rc);

		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	}
	else
	{
		RECT rc;
		POINT p1, p2;
		GetClientRect(g_hWnd, &rc);

		p1.x = rc.left;
		p1.y = rc.top;
		p2.x = rc.right;
		p2.y = rc.bottom;

		ClientToScreen(g_hWnd, &p1);
		ClientToScreen(g_hWnd, &p2);

		rc.left = p1.x;
		rc.top = p1.y;
		rc.right = p2.x;
		rc.bottom = p2.y;
		ClipCursor(&rc);

		GetCursorPos(&p1);
		ScreenToClient(g_hWnd, &p1);

		_float tmpX = (_float)p1.x - g_iWinSizeX * 0.5f;
		_float tmpY = g_iWinSizeY * 0.5f - (_float)p1.y;

		m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(tmpX, tmpY, 0.f, 1.f));

		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	}
}

void CUI_Crosshair::Tick(_float fTimeDelta)
{
}

void CUI_Crosshair::LateTick(_float fTimeDelta)
{
	if(!m_bHide)
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CUI_Crosshair::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (m_bFree || (LEVEL_STAGE1 != GET_CURLEVEL && LEVEL_STAGE2 != GET_CURLEVEL && LEVEL_STAGE1_BOSS != GET_CURLEVEL))
	{
		if (FAILED(m_pTextureComBasic->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Crosshair::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Crosshair"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"),
		TEXT("Com_Texture_Basic"), reinterpret_cast<CComponent**>(&m_pTextureComBasic))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Crosshair * CUI_Crosshair::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Crosshair*		pInstance = new CUI_Crosshair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Crosshair"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_Crosshair::Clone(void * pArg)
{
	CUI_Crosshair*		pInstance = new CUI_Crosshair(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Crosshair"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Crosshair::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureComBasic);
	Safe_Release(m_pVIBufferCom);
}
