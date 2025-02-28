#include "Particle_BoxOpen.h"

#include "GameInstance.h"
#include "PipeLine.h"

CParticle_BoxOpen::CParticle_BoxOpen(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle_BoxOpen::CParticle_BoxOpen(const CParticle_BoxOpen & rhs)
	: CGameObject( rhs )
{
}

HRESULT CParticle_BoxOpen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_BoxOpen::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&static_cast<CTransform::TRANSFORM_DESC*>(pArg)->vInitPos));

	m_pTextureColor = static_cast<CTransform::TRANSFORM_DESC*>(pArg)->iCurrentCellIndex;

	m_pTransformCom->Rotation({ 0.f, 0.f, 1.f }, m_pTextureColor / 4);

	return S_OK;
}

void CParticle_BoxOpen::PriorityTick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;

	if (m_fDelayTime >= 3.f)
		Set_Dead(true);

	m_pVIBufferCom->Update_GoUp(fTimeDelta);
	m_pVIBufferCom->Update_Spread(fTimeDelta);
}

void CParticle_BoxOpen::Tick(_float fTimeDelta)
{
	
}

void CParticle_BoxOpen::LateTick(_float fTimeDelta)
{
	m_pTransformCom->Set_Billboard();

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CParticle_BoxOpen::Render()
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

HRESULT CParticle_BoxOpen::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_loop_ring"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Box"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_BoxOpen::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_pTextureColor)))
		return E_FAIL;

	return S_OK;
}

CParticle_BoxOpen * CParticle_BoxOpen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle_BoxOpen*		pInstance = new CParticle_BoxOpen(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_BoxOpen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CParticle_BoxOpen::Clone(void * pArg)
{
	CParticle_BoxOpen*		pInstance = new CParticle_BoxOpen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_BoxOpen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_BoxOpen::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
