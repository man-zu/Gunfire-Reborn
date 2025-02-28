#include "Monster_BoomPart.h"

#include "GameInstance.h"
#include "Player.h"
#include "Effect_Common.h"
#include "Monster.h"
#include "Level.h"

CMonster_BoomPart::CMonster_BoomPart(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext } 
{
}

CMonster_BoomPart::CMonster_BoomPart(const CMonster_BoomPart & rhs)
	: CPartObject(rhs)
{
}

const _float4x4 * CMonster_BoomPart::Get_BoneMatrixPtr(const _char * pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);	
}

HRESULT CMonster_BoomPart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_BoomPart::Initialize(void * pArg)
{
	m_eBoomState = BOOM_MOVE;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Navi(pArg)))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0, true);

	/* 최초 위치 설정 */
	m_pPosMove = { -35.f, 0.f, -27.5f, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pPosMove));
	m_pTransformCom->Set_Scale({ 1.2f, 1.2f, 1.2f });

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());
	m_fAttack = 30.f;

	return S_OK;
}

void CMonster_BoomPart::PriorityTick(_float fTimeDelta)
{
	if (m_eBoomState == BOOM)
	{ 
		m_fDelayTime += fTimeDelta;
		if (m_fDelayTime >= 1.f)
			m_eBoomState = BOOM_DIE;
	}
}

void CMonster_BoomPart::Tick(_float fTimeDelta)
{

	if (KEY_HOLD(DIK_B))
		m_eBoomState = BOOM_MOVE;

	motion_moveToPlayer(fTimeDelta);
	motion_boom();
	motion_idle();

	m_pModelCom->Play_Animation(fTimeDelta);


	/* SET NAVI HEIGHT */
	_float4 vCurPos;
	XMStoreFloat4(&vCurPos, m_pParentTransform->Get_State(CTransform::STATE_POSITION));
	if(m_pNavigationCom != nullptr)
	{
		_float fNaviHeight = m_pNavigationCom->GetHeight(m_pParentTransform->Get_State(CTransform::STATE_POSITION), m_pNavigationCom->GetCurrIdx());
		vCurPos.y = 0.f + fNaviHeight;
	}
	m_pParentTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vCurPos));

	/* UPDATE PART TRANSFORM */
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

}

void CMonster_BoomPart::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pColliderCom->Update(m_pParentTransform->Get_WorldMatrix());
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_SHADOWGAMEOBJECT, this);
}

HRESULT CMonster_BoomPart::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CMonster_BoomPart::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_boom"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingAABBDesc{};
	
	BoundingAABBDesc.vExtents = _float3(.5f, .7f, .5f);
	BoundingAABBDesc.vCenter = _float3(0.f, BoundingAABBDesc.vExtents.y, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingAABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_BoomPart::Ready_Navi(void* pArg)
{
	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC		NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = static_cast<CMonster::MONSTER_DESC*>(pArg)->iCurrentCellIndex;

	if (static_cast<CMonster::MONSTER_DESC*>(pArg)->eLevel == LEVEL_STAGE1)
	{
		if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Prototype_Component_Navigation_Stage1"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}
	if (static_cast<CMonster::MONSTER_DESC*>(pArg)->eLevel == LEVEL_STAGE2)
	{
		if (FAILED(__super::Add_Component(LEVEL_STAGE2, TEXT("Prototype_Component_Navigation_Stage2"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
	}
	if (GET_CURLEVEL == LEVEL_STAGE1_BOSS)
	{
		if (FAILED(__super::Add_Component(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Navigation_StageBoss"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
			return E_FAIL;
		m_pNavigationCom = nullptr;
	}
	return S_OK;
}

HRESULT CMonster_BoomPart::Bind_ShaderResources()
{
	_float4x4 mat, matInv;
	XMStoreFloat4x4(&mat, XMMatrixIdentity());
	XMStoreFloat4((_float4*)&mat.m[3][0], { -35.f, 0.f, -27.5f, 1.f });
	XMStoreFloat4x4(&matInv, XMMatrixInverse(nullptr, XMLoadFloat4x4(&mat)));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_partMatrix", &mat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_partMatrixInv", &matInv)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float3 camPos = (_float3)m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW)->m[3];
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamPos", &camPos, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRedRim", &m_bRim, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bDead", &m_bDissolve, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

void CMonster_BoomPart::motion_moveToPlayer(_float fTimeDelta)
{
	if (m_eBoomState != BOOM_MOVE)
		return;

	if (0 == m_iStateCurNum)
	{
		m_pParentTransform->LookAt(m_pPlayer->GetPlayerPos());
		m_pModelCom->Set_AnimIndex(4, true);
		m_iStateCurNum = 1;
	}

	// 플레이어랑 가까워지면 달리기 stop
	if (1 == m_iStateCurNum)
	{
		m_pParentTransform->Go_Target(fTimeDelta, m_pPlayer->GetPlayerPos(), m_pNavigationCom);

		if (m_pParentTransform->Go_DistanceCheck(m_pPlayer->GetPlayerPos(), 5.f))
		{
			m_iStateCurNum = 2;
		}
	}

	if (2 == m_iStateCurNum)
	{
		m_pModelCom->Set_AnimStop();
		m_eBoomState = BOOM_ATTACK;
		m_iStateCurNum = 0;
	}
}

/**************** spin -> boom ****************/ 
void CMonster_BoomPart::motion_boom()
{
	if (m_eBoomState != BOOM_ATTACK)
		return;

	if (0 == m_iStateCurNum)
	{
		m_iStateCurNum = 1;
		
		_uint tmp = rand() % 2;
		m_pModelCom->Set_AnimIndex(tmp+1, false);		// 1 or 2 (회전동작)
	}

	if (1 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_iStateCurNum = 2;
		m_pModelCom->Set_AnimIndex(3, false);			// 터지기
	}

	if (2 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		///* 효과 */
		//CEffect_Common::EFFECT_DESC effectDesc{};
		//XMStoreFloat4(&effectDesc.vInitPos, m_pParentTransform->Get_State(CTransform::STATE_POSITION));
		////effectDesc.vInitPos.y -= 1.f;
		//effectDesc.iImgCnt = 16;
		//effectDesc.vScale = { 3.f, 3.f, 3.f };
		//effectDesc.sTextureTag = L"Prototype_Component_Texture_Effect_Fire_02";
		//
		//FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Effect", L"Prototype_Component_Effect_Common", &effectDesc));

		motion_reset();
		m_eBoomState = BOOM;
	}
}

void CMonster_BoomPart::motion_idle()
{
	if (m_eBoomState != BOOM_IDLE)
		return;

	// 6 걷기
	m_pModelCom->Set_AnimIndex(0, true);
}

void CMonster_BoomPart::motion_reset()
{
	m_iStateCurNum = 0;
	m_eBoomState = BOOM_IDLE;
}

void CMonster_BoomPart::CheckColl()
{
	CPlayer* pTargetObject = m_pPlayer;
	if (nullptr == pTargetObject)
		return;
	
	CCollider* pTargetCollider = pTargetObject->GetPlayerColl();
	if (nullptr == pTargetCollider)
		return;
	
	if (m_pColliderCom->Intersect(pTargetCollider))
	{
		pTargetObject->SetDamaged(m_fAttack);
	}
}

CMonster_BoomPart * CMonster_BoomPart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster_BoomPart*		pInstance = new CMonster_BoomPart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_BoomPart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonster_BoomPart::Clone(void * pArg)
{
	CMonster_BoomPart*		pInstance = new CMonster_BoomPart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_BoomPart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMonster_BoomPart::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
}
