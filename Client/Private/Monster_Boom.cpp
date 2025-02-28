#include "Monster_Boom.h"

#include "GameInstance.h"
#include "Effect_Common.h"
#include "Monster_Effect.h"
#include "Player.h"

CMonster_Boom::CMonster_Boom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CMonster_Boom::CMonster_Boom(const CMonster_Boom& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonster_Boom::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Boom::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC trans;
	trans.fSpeedPerSec = 5.f;
	trans.fRotationPerSec = 5.f;

 	if (FAILED(__super::Initialize(&trans)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects(pArg)))
		return E_FAIL;

	_float4 initPos = static_cast<CTransform::TRANSFORM_DESC*>(pArg)->vInitPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&initPos));

	m_iHp = m_iMaxHp = 30;
	m_fShield = m_fMaxShield = 0.f;


	LIGHT_DESC			LightDesc{};
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = vPos;
	LightDesc.fRange = 5.f;

	LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	return S_OK;
}

void CMonster_Boom::PriorityTick(_float fTimeDelta)
{
	if (m_iHp <= 0)
		m_pBody->SetCurBoomState(BOOM_DIE);

	if (m_pBody != nullptr && BOOM_DIE == m_pBody->GetCurBoomState())
	{
		Set_Dead(true);
		dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front())->AddKillCnt();

		/* 효과 */
		if (m_iHp <= 0)
		{
			CMonster_Effect::MON_EFFECT_DESC effectDesc{};
			XMStoreFloat4(&effectDesc.vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			effectDesc.pNavi = m_pBody->GetNavigation();
			FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Effect", L"Prototype_Component_GameObject_Monster_Effect", &effectDesc));
		}
		else
		{
			CEffect_Common::EFFECT_DESC effectDesc{};
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			XMStoreFloat4(&effectDesc.vInitPos, { vPos.m128_f32[0], vPos.m128_f32[1] + 1.3f, vPos.m128_f32[2], vPos.m128_f32[3], });
			effectDesc.iImgCnt = 16;
			effectDesc.vScale = { 5.f, 5.f, 5.f };
			effectDesc.sTextureTag = L"Prototype_Component_Texture_Effect_Fire_02";

			FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Effect", L"Prototype_Component_Effect_Common", &effectDesc));

			if (m_pTransformCom->Go_DistanceCheck(dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front())->GetPlayerPos(), 10.f))
				dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front())->SetDamaged(30);
		}
	}

	for (auto& pPartObj : m_Parts)
		pPartObj->PriorityTick(fTimeDelta);
}

void CMonster_Boom::Tick(_float fTimeDelta)
{
	for (auto& pPartObj : m_Parts)
		pPartObj->Tick(fTimeDelta);
}

void CMonster_Boom::LateTick(_float fTimeDelta)
{
	for (auto& pPartObj : m_Parts)
		pPartObj->LateTick(fTimeDelta);

	LandObjectMap();
	
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMonster_Boom::Render()
{
	return S_OK;
}

HRESULT CMonster_Boom::Ready_PartObjects(void* pArg)
{
	/* 몬스터 모델 */
	MONSTER_DESC		BodyDesc{};
	
	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.fSpeedPerSec = 5.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.iCurrentCellIndex = static_cast<MONSTER_DESC*>(pArg)->iCurrentCellIndex;
	BodyDesc.eLevel = static_cast<MONSTER_DESC*>(pArg)->eLevel;
	
	 m_pBody = dynamic_cast<CMonster_BoomPart*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Monster_BoomPart"), &BodyDesc));
	if (nullptr == m_pBody)
		return E_FAIL;
	m_Parts.emplace_back(m_pBody);

	return S_OK;
}

HRESULT CMonster_Boom::Ready_Components()
{
	return S_OK;
}

void CMonster_Boom::LandObjectMap()
{
	_float4 pos;
	XMStoreFloat4(&pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	pos.y = -.9f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pos));
}

CMonster_Boom* CMonster_Boom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Boom* pInstance = new CMonster_Boom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Boom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CMonster_Boom::Clone(void* pArg)
{
	CMonster_Boom* pInstance = new CMonster_Boom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Boom"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Boom::Free()
{
	__super::Free();

	for (auto& pPartObj : m_Parts)
		Safe_Release(pPartObj);

	Safe_Release(m_pNavigationCom);
}
