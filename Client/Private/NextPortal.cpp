#include "NextPortal.h"

#include "GameInstance.h"
#include "LoadModel.h"
#include "Player.h"
#include "Level.h"
#include "UI_Etc_NextStage.h"
#include "Level_Loading.h"

CNextPortal::CNextPortal(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CNextPortal::CNextPortal(const CNextPortal & rhs)
	: CGameObject( rhs )
{
}

HRESULT CNextPortal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNextPortal::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC* TransformDesc = static_cast<CTransform::TRANSFORM_DESC*>(pArg);

	if (FAILED(__super::Initialize(TransformDesc)))
		return E_FAIL;
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&TransformDesc->vInitPos));
	m_pTransformCom->Rotation({0.f, 1.f, 0.f}, XMConvertToRadians(TransformDesc->fTurn));
	m_pTransformCom->Set_Scale({ 4.5f, 4.5f, 4.5f });
	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	return S_OK;
}

void CNextPortal::PriorityTick(_float fTimeDelta)
{
	if (GET_CURLEVEL == LEVEL_STAGE1)
	{
		if (0 == m_iMonCnt)
			m_iMonCnt = 12;

		if (m_bPortal)
			m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, fTimeDelta);
	}
	else if (GET_CURLEVEL == LEVEL_STAGE2)
	{
		if (0 == m_iMonCnt)
			m_iMonCnt = 10;

		if (m_bPortal)
			m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, fTimeDelta);
	}
}

void CNextPortal::Tick(_float fTimeDelta)
{
	_uint iPlayerKillCnt = m_pPlayer->GetKillCnt();

	if (iPlayerKillCnt == m_iMonCnt)
		m_bPortal = true;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	if (m_bPortal)		// 몬스터 유효마리수 처지
		CheckColl();
}

void CNextPortal::LateTick(_float fTimeDelta)
{
	if (m_bPortal)
		m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CNextPortal::Render()
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

HRESULT CNextPortal::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_portal"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingAABBDesc{};

	BoundingAABBDesc.vExtents = _float3(.5f, .5f, .5f);
	BoundingAABBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingAABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNextPortal::Bind_ShaderResources()
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

void CNextPortal::CheckColl()
{
	if (!m_bPortal)
		return;

	CPlayer* pTargetObject = m_pPlayer;
	if (nullptr == pTargetObject)
		return;

	CCollider* pTargetCollider = pTargetObject->GetPlayerColl();
	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		m_bOpenTextShow = true;
		dynamic_cast<CUI_Etc_NextStage*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_NextStage")->front())->SetShowYN(true);
	}
	else
		dynamic_cast<CUI_Etc_NextStage*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_NextStage")->front())->SetShowYN(false);
}

CNextPortal * CNextPortal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNextPortal*		pInstance = new CNextPortal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CNextPortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CNextPortal::Clone(void * pArg)
{
	CNextPortal*		pInstance = new CNextPortal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNextPortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNextPortal::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPlayer);

}
