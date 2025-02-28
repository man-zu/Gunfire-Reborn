
#include "Player_Effect_Dash.h"
#include "GameInstance.h"
#include "Level.h"

CPlayer_Effect_Dash::CPlayer_Effect_Dash(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer_Effect_Dash::CPlayer_Effect_Dash(const CPlayer_Effect_Dash & rhs)
	: CGameObject( rhs )
{
}

HRESULT CPlayer_Effect_Dash::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Effect_Dash::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;
	
	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX * 1.5f;
	m_fSizeY = g_iWinSizeY * 1.5f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.5f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	
	return S_OK;
}

void CPlayer_Effect_Dash::PriorityTick(_float fTimeDelta)
{
}

void CPlayer_Effect_Dash::Tick(_float fTimeDelta)
{
}

void CPlayer_Effect_Dash::LateTick(_float fTimeDelta)
{
	if (m_iIdx > 4)
	{
		m_iIdx = 0;
		m_bShow = false;
	}
	else
	{
		m_pDelayTime += fTimeDelta;
		if (m_pDelayTime >= 0.05f)
		{
			m_iIdx++;
			m_pDelayTime = 0.f;
		}
	}

	if (!m_bShow)
		return;

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer_Effect_Dash::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Effect_Dash::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dash_Effect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CPlayer_Effect_Dash * CPlayer_Effect_Dash::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Effect_Dash*		pInstance = new CPlayer_Effect_Dash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Effect_Dash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer_Effect_Dash::Clone(void * pArg)
{
	CPlayer_Effect_Dash*		pInstance = new CPlayer_Effect_Dash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_Effect_Dash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Effect_Dash::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
