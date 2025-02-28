#include "UI_Boss65ShieldBar.h"

#include "GameInstance.h"
#include "Boss.h"
#include "Level.h"

CUI_Boss65ShieldBar::CUI_Boss65ShieldBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Boss65ShieldBar::CUI_Boss65ShieldBar(const CUI_Boss65ShieldBar & rhs)
	: CUI( rhs )
{
}

HRESULT CUI_Boss65ShieldBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Boss65ShieldBar::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_tUIDesc.fSizeX = 415.f;
	m_tUIDesc.fSizeY = 8.f;
	m_tUIDesc.fX = -(g_iWinSizeX * 0.5f) + 225.f + 388.f - m_tUIDesc.fSizeX * 0.5f + 80.f;
	m_tUIDesc.fY = (g_iWinSizeY * 0.5f) - 95.f;

	m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, 0.2f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI_Boss65ShieldBar::PriorityTick(_float fTimeDelta)
{
	if (GET_CURLEVEL != LEVEL_STAGE1_BOSS)
		return;

	if (!m_pBoss)
		m_pBoss = dynamic_cast<CBoss*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_Monster", 0));

	m_fQuantity = _float(m_pBoss->GetShield()) / _float(m_pBoss->GetMaxShield());

}

void CUI_Boss65ShieldBar::Tick(_float fTimeDelta)
{
}

void CUI_Boss65ShieldBar::LateTick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX * m_fQuantity + 0.1f, m_tUIDesc.fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet((m_tUIDesc.fX + (m_tUIDesc.fSizeX * m_fQuantity * 0.5f)), m_tUIDesc.fY, 0.f, 1.f));

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Boss65ShieldBar::Render()
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

HRESULT CUI_Boss65ShieldBar::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Character_SFBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Boss65ShieldBar * CUI_Boss65ShieldBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Boss65ShieldBar*		pInstance = new CUI_Boss65ShieldBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Boss65ShieldBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_Boss65ShieldBar::Clone(void * pArg)
{
	CUI_Boss65ShieldBar*		pInstance = new CUI_Boss65ShieldBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Boss65ShieldBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Boss65ShieldBar::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
