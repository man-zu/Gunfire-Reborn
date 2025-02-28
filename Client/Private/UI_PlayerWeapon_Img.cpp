#include "UI_PlayerWeapon_Img.h"

#include "GameInstance.h"
#include "player.h"

CUI_PlayerWeapon_Img::CUI_PlayerWeapon_Img(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_PlayerWeapon_Img::CUI_PlayerWeapon_Img(const CUI_PlayerWeapon_Img & rhs)
	: CUI( rhs )
{
}

HRESULT CUI_PlayerWeapon_Img::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerWeapon_Img::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_tUIDesc.fSizeX = 256.f * 0.5f;
	m_tUIDesc.fSizeY = 256.f * 0.5f;
	m_tUIDesc.fX = -(g_iWinSizeX * 0.5f) + 1150.f;
	m_tUIDesc.fY = (g_iWinSizeY * 0.5f) - 630.f;

	m_pTransformCom->Set_Scale(_float3(m_tUIDesc.fSizeX, m_tUIDesc.fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIDesc.fX, m_tUIDesc.fY, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	return S_OK;
}

void CUI_PlayerWeapon_Img::PriorityTick(_float fTimeDelta)
{
}

void CUI_PlayerWeapon_Img::Tick(_float fTimeDelta)
{
	m_pCurWeapon = m_pPlayer->GetWeaponType();
}

void CUI_PlayerWeapon_Img::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_UI, this);
}

HRESULT CUI_PlayerWeapon_Img::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (_uint)m_pCurWeapon - 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerWeapon_Img::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Weapon_Img"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_PlayerWeapon_Img * CUI_PlayerWeapon_Img::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_PlayerWeapon_Img*		pInstance = new CUI_PlayerWeapon_Img(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_PlayerWeapon_Img"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUI_PlayerWeapon_Img::Clone(void * pArg)
{
	CUI_PlayerWeapon_Img*		pInstance = new CUI_PlayerWeapon_Img(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_PlayerWeapon_Img"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerWeapon_Img::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
