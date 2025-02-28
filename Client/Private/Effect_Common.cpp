#include "Effect_Common.h"

#include "GameInstance.h"

#include "Level.h"

CEffect_Common::CEffect_Common(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CEffect_Common::CEffect_Common(const CEffect_Common & rhs)
	: CGameObject( rhs )
{

}

HRESULT CEffect_Common::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Common::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;


	EFFECT_DESC* effectDesc = static_cast<EFFECT_DESC*>(pArg);
	
	if (FAILED(Ready_Components(effectDesc->sTextureTag)))
		return E_FAIL;

	m_fSpeed = effectDesc->fSpeed;
	m_iImgCnt = effectDesc->iImgCnt;
	m_bIsLoop = effectDesc->bIsLoop;
	m_iLoopTime = effectDesc->iLoopfTime;

	if (effectDesc->pWorldTransform._11 != 0.f)
		m_pTransformCom->Set_WorldMat(effectDesc->pWorldTransform);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&effectDesc->vInitPos));
	m_pTransformCom->Set_Scale(effectDesc->vScale);
	m_pTransformCom->Go_Straight(effectDesc->MoveAmount.x);
	m_pTransformCom->Go_Right(effectDesc->MoveAmount.y);
	m_pTransformCom->Go_Left(effectDesc->MoveAmount.z);
	m_pTransformCom->Go_Down(effectDesc->MoveAmount.w);

	m_pTransformCom->Rotation({ 1.f, 0.f, 0.f }, XMConvertToRadians(effectDesc->iRotRadian));

	return S_OK;
}

void CEffect_Common::PriorityTick(_float fTimeDelta)
{
}

void CEffect_Common::Tick(_float fTimeDelta)
{
	m_fFrame += m_iImgCnt * fTimeDelta * m_fSpeed;

	if(m_bIsLoop == false)
	{
		if (m_fFrame >= m_iImgCnt)
		{
			m_fFrame = 0.f;
			Set_Dead(true);
		}
	}

	if (m_bIsLoop == true)
	{
		if (m_iLoopTime == m_iLoopCount)
			Set_Dead(true);

		if (m_fFrame >= m_iImgCnt)
		{
			m_fFrame = 0.f;
			m_iLoopCount++;
		}
	}
}

void CEffect_Common::LateTick(_float fTimeDelta)
{
	if(!m_bIsLoop)
		m_pTransformCom->Set_Billboard();
	
	if(!m_bIsDead)
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CEffect_Common::Render()
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

HRESULT CEffect_Common::Ready_Components(wstring sTextureTag)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, sTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Common::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", _uint(m_fFrame))))
		return E_FAIL;

	return S_OK;
}

CEffect_Common * CEffect_Common::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_Common*		pInstance = new CEffect_Common(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CEffect_Common::Clone(void * pArg)
{
	CEffect_Common*		pInstance = new CEffect_Common(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Common::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

}
