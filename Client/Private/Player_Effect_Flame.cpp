#include "Player_Effect_Flame.h"

#include "GameInstance.h"
#include "Model.h"

#include "Player_Body.h"
#include "Camera_Player.h"

CPlayer_Effect_Flame::CPlayer_Effect_Flame(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CPlayer_Effect_Flame::CPlayer_Effect_Flame(const CPlayer_Effect_Flame & rhs)
	: CPartObject( rhs )
{
}

HRESULT CPlayer_Effect_Flame::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Effect_Flame::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(100.f, 100.f, 0.f, 1.f));

	return S_OK;
}

void CPlayer_Effect_Flame::PriorityTick(_float fTimeDelta)
{
}

void CPlayer_Effect_Flame::Tick(_float fTimeDelta)
{
	//m_pVIBufferCom->Update_Drop(fTimeDelta);
	m_pTransformCom->Turn({0.f, 0.f, 1.f}, XMConvertToRadians(5.f));
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_Effect_Flame::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CPlayer_Effect_Flame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Effect_Flame::Ready_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PlayerWeapon_shot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Effect_Flame::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CPlayer_Effect_Flame * CPlayer_Effect_Flame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Effect_Flame*		pInstance = new CPlayer_Effect_Flame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Effect_Flame"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer_Effect_Flame::Clone(void * pArg)
{
	CPlayer_Effect_Flame*		pInstance = new CPlayer_Effect_Flame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_Effect_Flame"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Effect_Flame::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
