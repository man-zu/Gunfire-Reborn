#include "Arrow.h"

#include "GameInstance.h"
#include "LoadModel.h"
#include "Level.h"
#include "player.h"

CArrow::CArrow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CArrow::CArrow(const CArrow & rhs)
	: CGameObject( rhs )
{
}

HRESULT CArrow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CArrow::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC*		TransformDesc = static_cast<CTransform::TRANSFORM_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&TransformDesc->vInitPos));
	m_pTransformCom->Set_Scale({0.25f, 0.25f, 0.25f });

	m_fAttack = MONSTER_ARROW_DAMAGE;

	_vector vTargetPos = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0))->GetPlayerPos();
	m_pTransformCom->LookAt(vTargetPos);

	vTargetPos.m128_f32[1] = -0.4f;

	m_pDir = XMVector3Normalize(vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

void CArrow::PriorityTick(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;

	if (m_fDelayTime >= 10.f)
		Set_Dead(true);

	m_pTransformCom->Go_TargetDir(fTimeDelta, m_pDir);
}

void CArrow::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	CheckColl();
}

void CArrow::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CArrow::Render()
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

void CArrow::CheckColl()
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

		pTargetObject->SetDamaged(m_fAttack);
	}
}

HRESULT CArrow::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Weapon_Arrow"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	CollDesc{};

	CollDesc.fRadius = 2.5f;
	CollDesc.vCenter = _float3(0.f, CollDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CArrow::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CArrow * CArrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CArrow*		pInstance = new CArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CArrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CArrow::Clone(void * pArg)
{
	CArrow*		pInstance = new CArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CArrow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArrow::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
