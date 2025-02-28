#include "Coin.h"

#include "GameInstance.h"
#include "LoadModel.h"
#include "Level.h"
#include "player.h"

CCoin::CCoin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CCoin::CCoin(const CCoin & rhs)
	: CGameObject( rhs )
{
}

HRESULT CCoin::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoin::Initialize(void * pArg)
{
	COIN_DESC*		TransformDesc = static_cast<COIN_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(TransformDesc->iCurrentCellIndex)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&TransformDesc->vInitPos));
	m_pTransformCom->Set_Scale({0.15f, 0.15f, 0.15f });

	/* x,z 축 랜덤 이동 세팅 */
	iRandX = _float(rand() % 10) / _float(100.f);
	iRandZ = _float(rand() % 10) / _float(100.f);
	iSign = rand() % 2 == 0 ? 1 : -1;

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	return S_OK;
}

void CCoin::PriorityTick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;
	m_pTransformCom->Turn({0.f, 1.f, 0.f}, fTimeDelta);

	PopMotion(fTimeDelta);

	if (g_bCoinAuto)
		moveToPlayer(fTimeDelta);
}

void CCoin::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	CheckColl();
}

void CCoin::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	/* 렌더러클래스에 디버그용 렌더그룹에 추가해주고 디퍼드단계가 아닌 위치에서 컴포넌트들을 그릴 수 있도록 하게싿. */
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CCoin::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < 1; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CCoin::CheckColl()
{
	CPlayer* pTargetObject = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
	if (nullptr == pTargetObject)
		return;
	
	CCollider* pTargetCollider = pTargetObject->GetPlayerColl();
	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		Set_Dead(true);
		PLAY(L"getItem", false);

		pTargetObject->CoinAdd(_uint(rand() % 3 + 1) * 20);
	}
}

void CCoin::PopMotion(_float fTimeDelta)
{
	if (!m_bPop)
		return;

	/* y 축 세팅 */
	_float fGravity = 7.5f;
	_float fJumpPower = .4f;

	_float4 fPos;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fJumpTime += fTimeDelta;

	fPos.y += (fJumpPower * fJumpPower) - (fGravity * m_fJumpTime * m_fJumpTime * 0.5f);

	if (fPos.y <= m_pNavigationCom->GetHeight(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pNavigationCom->GetCurrIdx()))
	{
		fPos.y = m_pNavigationCom->GetHeight(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pNavigationCom->GetCurrIdx());
		m_fJumpTime = 0.f;
		m_bPop = false;
	}

	m_pTransformCom->Go_Right(iRandX * iSign, m_pNavigationCom);
	m_pTransformCom->Go_Straight(iRandZ * iSign, m_pNavigationCom);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&fPos));
	// 퍼져서 드랍되도록
}

void CCoin::moveToPlayer(_float fTimeDelta)
{
	if (!m_pTransformCom->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 7.f))
		return;

	_vector target = m_pPlayer->GetPlayerPos();
	target.m128_f32[1] -= .8f;
	m_pTransformCom->Go_Target(fTimeDelta, target);

}

HRESULT CCoin::Ready_Components(_uint iCellIdx)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_PropCoin"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	CollDesc{};

	CollDesc.fRadius = 2.5f;
	CollDesc.vCenter = _float3(0.f, CollDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC		NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = iCellIdx;

	if (GET_CURLEVEL == LEVEL_STAGE1)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Prototype_Component_Navigation_Stage1"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}
	if (GET_CURLEVEL == LEVEL_STAGE2)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_STAGE2, TEXT("Prototype_Component_Navigation_Stage2"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}
	if (GET_CURLEVEL == LEVEL_STAGE1_BOSS)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Navigation_StageBoss"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCoin::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CCoin * CCoin::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCoin*		pInstance = new CCoin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCoin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CCoin::Clone(void * pArg)
{
	CCoin*		pInstance = new CCoin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCoin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCoin::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
