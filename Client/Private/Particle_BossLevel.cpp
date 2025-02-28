#include "Particle_BossLevel.h"

#include "GameInstance.h"
#include "PipeLine.h"

CParticle_BossLevel::CParticle_BossLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle_BossLevel::CParticle_BossLevel(const CParticle_BossLevel & rhs)
	: CGameObject( rhs )
{
}

HRESULT CParticle_BossLevel::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_BossLevel::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 20.f, 0.f,20.f, 1.f});

	return S_OK;
}

void CParticle_BossLevel::PriorityTick(_float fTimeDelta)
{
	m_pTextureIdx += fTimeDelta * 16.f;

	if (m_pTextureIdx >= 16.f)
		m_pTextureIdx = 0;
}

void CParticle_BossLevel::Tick(_float fTimeDelta)
{
	m_pVIBufferCom->Update_GoUp(fTimeDelta);
}

void CParticle_BossLevel::LateTick(_float fTimeDelta)
{
	m_pTransformCom->Set_Billboard();

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CParticle_BossLevel::Render()
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

HRESULT CParticle_BossLevel::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GW_fire"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_BossLevel"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_BossLevel::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (_uint)m_pTextureIdx)))
		return E_FAIL;

	return S_OK;
}

CParticle_BossLevel * CParticle_BossLevel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle_BossLevel*		pInstance = new CParticle_BossLevel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_BossLevel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CParticle_BossLevel::Clone(void * pArg)
{
	CParticle_BossLevel*		pInstance = new CParticle_BossLevel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_BossLevel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_BossLevel::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
