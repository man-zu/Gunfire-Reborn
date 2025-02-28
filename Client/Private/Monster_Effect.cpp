#include "Monster_Effect.h"

#include "GameInstance.h"
#include "Effect_Common.h"
#include "Coin.h"


CMonster_Effect::CMonster_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CMonster_Effect::CMonster_Effect(const CMonster_Effect & rhs)
	: CGameObject( rhs )
{

}

HRESULT CMonster_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Effect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pInitPos = static_cast<MON_EFFECT_DESC*>(pArg)->vInitPos;

	/* x,z 축 랜덤 이동 세팅 */

	// type 에 따라 효과 만드는거 다르게
	MakeGrayFog();

	if(nullptr != static_cast<MON_EFFECT_DESC*>(pArg)->pNavi)
		MakeItem(static_cast<MON_EFFECT_DESC*>(pArg)->pNavi);


	return S_OK;
}

void CMonster_Effect::MakeGrayFog()
{
	/* 안개 효과 */
	int iRand = rand() % 2 + 1;

	for (int i = 0; i < iRand; ++i)
	{
		iRandX = _float(rand() % 10) / 100.f + 1.f;
		iRandZ = _float(rand() % 10) / 100.f + 1.f;
		iSign = rand() % 2 == 0 ? 1 : -1;

		CEffect_Common::EFFECT_DESC effectDesc{};
		effectDesc.iImgCnt = 17;
		effectDesc.vScale = { 3.5f, 3.5f, 3.5f };
		effectDesc.sTextureTag = L"Prototype_Component_Texture_Effect_gray";
		effectDesc.vInitPos = { m_pInitPos.x + iRandX * iSign * i, m_pInitPos.y + 0.2f, m_pInitPos.z + iRandZ * iSign * i, 1.f };

		FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_Effect", L"Prototype_Component_Effect_Common", &effectDesc));
	}
}

void CMonster_Effect::MakeItem(CNavigation* pNavigationCom)
{
	int iRand = rand() % 3 + 1;

	for(int i=0;i<iRand;++i)
	{
		/* Drop Coin */
		CCoin::COIN_DESC desc{};
		desc.fSpeedPerSec = 10.f;
		desc.fRotationPerSec = XMConvertToRadians(90.0f);
		desc.iCurrentCellIndex = pNavigationCom->GetCurrIdx();
		desc.vInitPos = { m_pInitPos.x + 0.5f * i, m_pInitPos.y, m_pInitPos.z + 0.5f * i, 1.f};
		m_pGameInstance->Add_Clone(LEVEL_STAGE1, L"Layer_Obj_Coin", L"Prototype_Component_GameObject_Coin", &desc);
	}
}

HRESULT CMonster_Effect::Ready_Components()
{
	/* For.Com_Collider_Sphere */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	BoundingSphereDesc{};

	BoundingSphereDesc.fRadius = 0.25f;
	BoundingSphereDesc.vCenter = _float3(0.f, BoundingSphereDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingSphereDesc)))
		return E_FAIL;

	return S_OK;
}

void CMonster_Effect::CheckColl()
{
}

CMonster_Effect * CMonster_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster_Effect*		pInstance = new CMonster_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonster_Effect::Clone(void * pArg)
{
	CMonster_Effect*		pInstance = new CMonster_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

}
