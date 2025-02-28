#include "Monster_ShieldMan.h"

#include "GameInstance.h"
#include "Monster_Effect.h"
#include "Player.h"

CMonster_ShieldMan::CMonster_ShieldMan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CMonster_ShieldMan::CMonster_ShieldMan(const CMonster_ShieldMan& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonster_ShieldMan::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_ShieldMan::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC trans;
	trans.fSpeedPerSec = 2.f;
	trans.fRotationPerSec = 5.f;

 	if (FAILED(__super::Initialize(&trans)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPosMove = static_cast<CTransform::TRANSFORM_DESC*>(pArg)->vInitPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pPosMove));

	if (FAILED(Ready_PartObjects(pArg)))
		return E_FAIL;

	m_iHp = m_iMaxHp = 100;
	m_fShield = m_fMaxShield = 0.f;

	return S_OK;
}

void CMonster_ShieldMan::PriorityTick(_float fTimeDelta)
{
	if ( m_pBody!= nullptr && m_iHp <= 0)
	{
		m_fDelayTime += fTimeDelta;
		m_pBody->SetDissolve();
		m_pBody->SetDelayTime(fTimeDelta);
		m_pBody->Set_State(MONSTER_HIT);

		if (m_fDelayTime >= 2.f)
		{
			Set_Dead(true);
			dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front())->AddKillCnt();
			
		}

		/* Effect */
		if (!m_bDieEffect)
		{
			m_bDieEffect = true;
			PLAYONLY(L"ShieldManDie", false);

			CMonster_Effect::MON_EFFECT_DESC effectDesc{};
			XMStoreFloat4(&effectDesc.vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			effectDesc.pNavi = m_pBody->GetNavigation();
			FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Effect", L"Prototype_Component_GameObject_Monster_Effect", &effectDesc));
		}
	}

	for (auto& pPartObj : m_Parts)
		pPartObj->PriorityTick(fTimeDelta);
}

void CMonster_ShieldMan::Tick(_float fTimeDelta)
{
	for (auto& pPartObj : m_Parts)
		pPartObj->Tick(fTimeDelta);
}

void CMonster_ShieldMan::LateTick(_float fTimeDelta)
{
	for (auto& pPartObj : m_Parts)
		pPartObj->LateTick(fTimeDelta);

	LandObjectMap();
	
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMonster_ShieldMan::Render()
{
	return S_OK;
}

HRESULT CMonster_ShieldMan::Ready_PartObjects(void* pArg)
{
	/* Monster Model */
	MONSTER_DESC		BodyDesc{};
	
	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.fSpeedPerSec = 1.5f;
	BodyDesc.fRotationPerSec = 1.5f;
	BodyDesc.iCurrentCellIndex = static_cast<MONSTER_DESC*>(pArg)->iCurrentCellIndex;
	BodyDesc.eLevel = static_cast<MONSTER_DESC*>(pArg)->eLevel;
	
	 m_pBody = dynamic_cast<CMonster_ShieldManPart*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Monster_ShieldManPart"), &BodyDesc));
	if (nullptr == m_pBody)
		return E_FAIL;
	m_Parts.emplace_back(m_pBody);

	return S_OK;
}

HRESULT CMonster_ShieldMan::Ready_Components()
{

	return S_OK;
}

void CMonster_ShieldMan::LandObjectMap()
{
}

CMonster_ShieldMan* CMonster_ShieldMan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_ShieldMan* pInstance = new CMonster_ShieldMan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_ShieldMan"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CMonster_ShieldMan::Clone(void* pArg)
{
	CMonster_ShieldMan* pInstance = new CMonster_ShieldMan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_ShieldMan"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_ShieldMan::Free()
{
	__super::Free();

	for (auto& pPartObj : m_Parts)
		Safe_Release(pPartObj);
}
