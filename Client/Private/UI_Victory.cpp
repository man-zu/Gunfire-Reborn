#include "UI_Victory.h"

#include "GameInstance.h"

CUI_Victory::CUI_Victory(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Victory::CUI_Victory(const CUI_Victory & rhs)
	: CUI( rhs )
{
}

HRESULT CUI_Victory::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Victory::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_tUIDesc.fSizeX = 700.f * 0.02f;
	m_tUIDesc.fSizeY = 350.f * 0.02f;
	m_tUIDesc.fX = 0.f;
	m_tUIDesc.fY = 0.f;

	m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


	return S_OK;
}

void CUI_Victory::PriorityTick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;

	if(m_fDelayTime <= 1.3f)
		m_pTransformCom->Set_Scale({ 700.f * 0.65f * m_fDelayTime, 350.f * 0.65f * m_fDelayTime, 1.f });

	if (m_fDelayTime >= 3.5f)
	{
		m_bAlphaAdd = false;
		m_fAlphaChange += fTimeDelta * 0.2f;
		_float3 curScale = m_pTransformCom->Get_Scaled();
		m_pTransformCom->Set_Scale({ curScale.x * m_fAlphaChange * 10.f, curScale.y * m_fAlphaChange * 10.f, 1.f });
	}
}

void CUI_Victory::Tick(_float fTimeDelta)
{
}

void CUI_Victory::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Victory::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_AlphaChange", &m_fAlphaChange, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bAlphaAdd", &m_bAlphaAdd, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Victory::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTexAlpha"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Victory"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Victory * CUI_Victory::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Victory*		pInstance = new CUI_Victory(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Victory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_Victory::Clone(void * pArg)
{
	CUI_Victory*		pInstance = new CUI_Victory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Victory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Victory::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
