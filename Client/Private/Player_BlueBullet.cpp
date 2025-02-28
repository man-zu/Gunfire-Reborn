#include "Player_BlueBullet.h"

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

CPlayer_BlueBullet::CPlayer_BlueBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CPlayer_BlueBullet::CPlayer_BlueBullet(const CPlayer_BlueBullet & rhs)
	: CGameObject( rhs )
{

}

HRESULT CPlayer_BlueBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_BlueBullet::Initialize(void * pArg)
{
	m_fAttack = static_cast<PLAYER_BLUEBULLET*>(pArg)->fAttack;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMat(static_cast<CTransform::TRANSFORM_DESC*>(pArg)->worldMat);

	_vector pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_Scale({ 0.5f, 0.5f, 0.5f });
	m_pTransformCom->Go_InitXYZ(.01f, -.012f, .05f);

	TargetDir = static_cast<PLAYER_BLUEBULLET*>(pArg)->vTargetPos;

	return S_OK;
}

void CPlayer_BlueBullet::PriorityTick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;
	if (m_fDelayTime >= 1.5f)
		Set_Dead(true);

	m_pTransformCom->Go_Straight(fTimeDelta);
	m_pTransformCom->Go_Down(-fTimeDelta * 0.5f * TargetDir.m128_f32[0]);
	m_pTransformCom->Go_Right(-fTimeDelta * 0.5f * TargetDir.m128_f32[1]);
}

void CPlayer_BlueBullet::Tick(_float fTimeDelta)
{
	m_fFrame += 15.f * fTimeDelta;

	if (m_fFrame >= 15.f)
		m_fFrame = 0.f;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CheckColl();
}

void CPlayer_BlueBullet::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONLIGHT, this);

#ifdef _DEBUG
	/* ������Ŭ������ ����׿� �����׷쿡 �߰����ְ� ���۵�ܰ谡 �ƴ� ��ġ���� ������Ʈ���� �׸� �� �ֵ��� �ϰԚ�. */
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CPlayer_BlueBullet::Render()
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

HRESULT CPlayer_BlueBullet::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_blueShot"),
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

HRESULT CPlayer_BlueBullet::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CPlayer_BlueBullet::CheckColl()
{
	if (GET_CURLEVEL == LEVEL_STAGE1_BOSS)
	{
		/* ���� */
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
				/* ������ ��� : idle ���� && ��弦 �� ��� */ 
				if(i == 1 && pBoss->GetBossCurPtrnState() == BOSS_IDLE)
					pBoss->SetCurBossState(BOSS_DAMAGED);

				// ��弦�� �ƴϴ��� ������ ��
				pBoss->Attacked(m_fAttack);
				Set_Dead(true);

				CollEffect();
			}
		}
	}


	/* �պ��� */
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
		/* �� */
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
					pTargetObject->Set_State(MONSTER_HIT);
					Set_Dead(true);

					CollEffect();
				}
			}
		}

		/* Ȱ */
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
					pTargetObject->Set_State(MONSTER_HIT);
					Set_Dead(true);

					CollEffect();
				}
			}
		}

		/* ���� */
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
					pTargetObject->Set_State(MONSTER_HIT);
					Set_Dead(true);
				}
			}
		}
	}
}

void CPlayer_BlueBullet::CollEffect()
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

CPlayer_BlueBullet * CPlayer_BlueBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_BlueBullet*		pInstance = new CPlayer_BlueBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_BlueBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer_BlueBullet::Clone(void * pArg)
{
	CPlayer_BlueBullet*		pInstance = new CPlayer_BlueBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_BlueBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_BlueBullet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

}
