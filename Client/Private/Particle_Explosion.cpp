#include "Particle_Explosion.h"

#include "GameInstance.h"
#include "PipeLine.h"

CParticle_Explosion::CParticle_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle_Explosion::CParticle_Explosion(const CParticle_Explosion & rhs)
	: CGameObject( rhs )
{
}

HRESULT CParticle_Explosion::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Explosion::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;	
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&static_cast<CTransform::TRANSFORM_DESC*>(pArg)->vInitPos));

	return S_OK;
}

void CParticle_Explosion::PriorityTick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;

	if (m_fDelayTime >= 3.f)
		Set_Dead(true);

}

void CParticle_Explosion::Tick(_float fTimeDelta)
{
	m_pVIBufferCom->Update_Spread(fTimeDelta);
}

void CParticle_Explosion::LateTick(_float fTimeDelta)
{
	m_pTransformCom->Set_Billboard();

	//_float3		vScale = m_pTransformCom->Get_Scaled();
	//_float3		vRight = *(_float3*)&ViewMatrix.m[0][0];
	//_float3		vUp = *(_float3*)&ViewMatrix.m[1][0];
	//_float3		vLook = *(_float3*)&ViewMatrix.m[2][0];
	//
	//vRight = { vRight.x * vScale.x,  vRight.y * vScale.x, vRight.z * vScale.x };
	//vLook = { vLook.x * vScale.z, vLook.y * vScale.z, vLook.z * vScale.z };
	//m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMLoadFloat3(&vRight));
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&vLook));


	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CParticle_Explosion::Render()
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

HRESULT CParticle_Explosion::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Explosion::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CParticle_Explosion * CParticle_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle_Explosion*		pInstance = new CParticle_Explosion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Explosion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CParticle_Explosion::Clone(void * pArg)
{
	CParticle_Explosion*		pInstance = new CParticle_Explosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Explosion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Explosion::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
