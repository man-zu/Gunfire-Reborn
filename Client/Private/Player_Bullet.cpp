#include "Player_Bullet.h"

#include "GameInstance.h"
#include "Player.h"

#include "Level.h"

#include "Boss.h"
#include "Boss_65_Body.h"
#include "Monster_Boom.h"
#include "Monster_Sword.h"
#include "Monster_ShieldMan.h"
#include "Monster_Arrow.h"
#include "Effect_Common.h"

CPlayer_Bullet::CPlayer_Bullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CPlayer_Bullet::CPlayer_Bullet(const CPlayer_Bullet & rhs)
	: CGameObject( rhs )
{

}

HRESULT CPlayer_Bullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Bullet::Initialize(void * pArg)
{
	m_fAttack = static_cast<PLAYER_BULLET*>(pArg)->fAttack;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 플레이어 총구 기준에서 출발
	// 이미지는 (player 1인칭)카메라 기준으로 빌보드
	// 모델 없이 콜라이더 + 이미지
	m_pTransformCom->Set_WorldMat(static_cast<CTransform::TRANSFORM_DESC*>(pArg)->worldMat);

	_vector pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_Scale({ 0.5f, 0.5f, 0.5f });
	m_pTransformCom->Go_InitXYZ(.01f, -.012f, .05f);

	TargetPos = XMLoadFloat3((_float3*)&m_pGameInstance->Get_PickedPos());

	return S_OK;
}

void CPlayer_Bullet::PriorityTick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;
	if (m_fDelayTime >= 2.5f)
		Set_Dead(true);

	m_pTransformCom->Go_Straight(fTimeDelta);
	m_pTransformCom->Go_Down(-fTimeDelta * 0.1f);
	m_pTransformCom->Go_Right(-fTimeDelta * 0.1f);
}

void CPlayer_Bullet::Tick(_float fTimeDelta)
{

	m_fFrame += 15.f * fTimeDelta;

	if (m_fFrame >= 15.f)
		m_fFrame = 0.f;


	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CheckColl();
}

void CPlayer_Bullet::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);

#ifdef _DEBUG
	/* 렌더러클래스에 디버그용 렌더그룹에 추가해주고 디퍼드단계가 아닌 위치에서 컴포넌트들을 그릴 수 있도록 하게싿. */
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CPlayer_Bullet::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (_uint)m_fFrame)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Bullet::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Bullet_Basic_L"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Collider_Sphere */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	BoundingSphereDesc{};

	BoundingSphereDesc.fRadius = 0.25f;
	BoundingSphereDesc.vCenter = _float3(0.f, BoundingSphereDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingSphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Bullet::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Bullet::CheckColl()
{
	if (GET_CURLEVEL == LEVEL_STAGE1_BOSS)
	{
		/* 보스 */
		CBoss* pBoss = dynamic_cast<CBoss*>(m_pGameInstance->Find_GameObject(GET_CURLEVEL, TEXT("Layer_Monster"), 0));
		CBoss_65_Body* pTargetObject = pBoss->GetMyBodyPart();
		if (nullptr == pTargetObject)
			return;

		CCollider** pTargetCollider = pTargetObject->GetColl();
		if (nullptr == pTargetCollider)
			return;

		for(int i=0;i<4; ++i)
		{
			if (m_pColliderCom->Intersect(pTargetCollider[i]))
			{
				/* 데미지 모션 : idle 상태 && 헤드샷 일 경우 */ 
				if(i == 1 && pBoss->GetBossCurPtrnState() == BOSS_IDLE)
					pBoss->SetCurBossState(BOSS_DAMAGED);

				// 헤드샷이 아니더라도 데미지 들어감
				pBoss->Attacked(m_fAttack);
				Set_Dead(true);

				CollEffect();
			}
		}
	}


	/* 붐붐이 */
	CGameObject* objList = GET_OBJ(GET_CURLEVEL, TEXT("Layer_Monster_Boom"), 0);
	if (objList != nullptr)
	{
		list<CGameObject*>* objList = GET_LIST(GET_CURLEVEL, TEXT("Layer_Monster_Boom"));
		for (list<CGameObject*>::iterator iter = objList->begin(); iter != objList->end(); ++iter)
		{
			CMonster_Boom* pTargetObject = static_cast<CMonster_Boom*>(*iter);
			if (nullptr == pTargetObject)
				return;

			CCollider* pTargetCollider = pTargetObject->GetColl();
			if (nullptr == pTargetCollider)
				return;

			if (m_pColliderCom->Intersect(pTargetCollider))
			{
				pTargetObject->Attacked(m_fAttack);
				Set_Dead(true);
				CollEffect();
			}
		}
	}

	if (GET_CURLEVEL == LEVEL_STAGE1 || GET_CURLEVEL == LEVEL_STAGE2)
	{
		/* 검 */
		CGameObject* objList = GET_OBJ(GET_CURLEVEL, TEXT("Layer_Monster_Sword"), 0);
		if (objList != nullptr)
		{
			list<CGameObject*>* objList = GET_LIST(GET_CURLEVEL, TEXT("Layer_Monster_Sword"));
			for (list<CGameObject*>::iterator iter = objList->begin(); iter != objList->end(); ++iter)
			{
				CMonster_Sword* pTargetObject = static_cast<CMonster_Sword*>(*iter);
				if (nullptr == pTargetObject)
					return;

				CCollider* pTargetCollider = pTargetObject->GetColl();
				if (nullptr == pTargetCollider)
					return;

				if (m_pColliderCom->Intersect(pTargetCollider))
				{
					pTargetObject->Attacked(m_fAttack);
					PLAYONLY(L"swordDamaged", false);
					pTargetObject->Set_State(MONSTER_HIT);
					Set_Dead(true);

					CollEffect();
				}
			}
		}

		/* 활 */
		objList = GET_OBJ(GET_CURLEVEL, TEXT("Layer_Monster_Arrow"), 0);
		if (objList != nullptr)
		{
			list<CGameObject*>* objList = GET_LIST(GET_CURLEVEL, TEXT("Layer_Monster_Arrow"));
			for (list<CGameObject*>::iterator iter = objList->begin(); iter != objList->end(); ++iter)
			{
				CMonster_Arrow* pTargetObject = static_cast<CMonster_Arrow*>(*iter);
				if (nullptr == pTargetObject)
					return;

				CCollider* pTargetCollider = pTargetObject->GetColl();
				if (nullptr == pTargetCollider)
					return;

				if (m_pColliderCom->Intersect(pTargetCollider))
				{
					pTargetObject->Attacked(m_fAttack);
					PLAYONLY(L"ArrowDamaged", false);
					pTargetObject->Set_State(MONSTER_HIT);
					Set_Dead(true);

					CollEffect();
				}
			}
		}

		/* 방패 */
		objList = GET_OBJ(GET_CURLEVEL, TEXT("Layer_Monster_ShieldMan"), 0);
		if (objList != nullptr)
		{
			list<CGameObject*>* objList = GET_LIST(GET_CURLEVEL, TEXT("Layer_Monster_ShieldMan"));
			for (list<CGameObject*>::iterator iter = objList->begin(); iter != objList->end(); ++iter)
			{
				CMonster_ShieldMan* pTargetObject = static_cast<CMonster_ShieldMan*>(*iter);
				if (nullptr == pTargetObject)
					return;

				CCollider* pTargetColliderBody = pTargetObject->GetCollBody();
				CCollider* pTargetColliderShield = pTargetObject->GetCollShield();

				if (m_pColliderCom->Intersect(pTargetColliderShield))
				{
					Set_Dead(true);
				}
				else if (m_pColliderCom->Intersect(pTargetColliderBody))
				{
					pTargetObject->Attacked(m_fAttack);
					PLAYONLY(L"ShieldManDamaged", false);
					pTargetObject->Set_State(MONSTER_HIT);
					Set_Dead(true);
				}
			}
		}
	}
}

void CPlayer_Bullet::CollEffect()
{
	CEffect_Common::EFFECT_DESC effectDesc{};
	effectDesc.iImgCnt = 4;
	effectDesc.vScale = { 1.f, 1.f, 1.f };
	effectDesc.sTextureTag = L"Prototype_Component_Texture_Effect_DamagedFlash";
	effectDesc.bIsLoop = false;
	effectDesc.fSpeed = 1.5f;
	XMStoreFloat4(&effectDesc.vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Effect", L"Prototype_Component_Effect_Common", &effectDesc));
}

CPlayer_Bullet * CPlayer_Bullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Bullet*		pInstance = new CPlayer_Bullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer_Bullet::Clone(void * pArg)
{
	CPlayer_Bullet*		pInstance = new CPlayer_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_Bullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Bullet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

}
