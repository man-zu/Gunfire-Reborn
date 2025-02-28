#include "BoxPart.h"

#include "GameInstance.h"
#include "LoadModel.h"
#include "Player.h"
#include "UI_Etc_OpenBox.h"
#include "UI_Etc_BoxItem.h"
#include "UI_Crosshair.h"
#include "Level.h"
#include "Effect_Common.h"

CBoxPart::CBoxPart(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBoxPart::CBoxPart(const CBoxPart & rhs)
	: CPartObject( rhs )
{
}

HRESULT CBoxPart::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoxPart::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 최초 위치 설정 */
	_float4 pPosMove = { -38.3f, .5f, -19.8f, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pPosMove));

	m_pModelCom->Set_AnimIndex(0, true);

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	return S_OK;
}

void CBoxPart::PriorityTick(_float fTimeDelta)
{
	if (0 == m_iMonCnt && GET_CURLEVEL == LEVEL_STAGE1)
		m_iMonCnt = 5;
	else if (0 == m_iMonCnt && GET_CURLEVEL == LEVEL_STAGE2)
		m_iMonCnt = 7;

	_uint iPlayerKillCnt = m_pPlayer->GetKillCnt();

	if (iPlayerKillCnt == m_iMonCnt)
		m_bOpenTextShow = true;

	if (KEY_PUSH(DIK_G))
	{
		m_bOpen = true;
		PLAYONLY(L"BoxOpen", false);
	}

	if(m_bOpen)
		openBox(fTimeDelta);

}

void CBoxPart::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom->Update(m_pParentTransform->Get_WorldMatrix());
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	if (m_bOpenTextShow)		// 몬스터 유효마리수 처지
		CheckColl();
}

void CBoxPart::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBoxPart::Render()
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

HRESULT CBoxPart::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Box"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	
	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingAABBDesc{};

	BoundingAABBDesc.vExtents = _float3(3.f, 3.f, 3.f);
	BoundingAABBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingAABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoxPart::Bind_ShaderResources()
{
	_float4x4 mat, matInv;
	XMStoreFloat4x4(&mat, XMMatrixIdentity());
	XMStoreFloat4((_float4*)&mat.m[3][0], { -38.3f, .5f, -19.8f, 1.f });
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

	return S_OK;
}

void CBoxPart::openBox(_float fTimeDelta)
{
	m_bOpenTextShow = false;
	dynamic_cast<CUI_Etc_OpenBox*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_OpenBox")->front())->SetShowYN(false);
	
	if (0 == m_iStateCurNum)
	{
		m_pModelCom->Set_AnimIndex(1, false);
		m_iStateCurNum = 1;
	}

	if (1 == m_iStateCurNum && m_pModelCom->Get_Finished())
	{
		m_pModelCom->Set_AnimIndex(2, true);
		m_iStateCurNum = 2;

		CTransform::TRANSFORM_DESC		Desc{};
		Desc.vInitPos = { XMVectorGetX(m_pParentTransform->Get_State(CTransform::STATE_POSITION)),
						 XMVectorGetY(m_pParentTransform->Get_State(CTransform::STATE_POSITION)) + 1.f,
						 XMVectorGetZ(m_pParentTransform->Get_State(CTransform::STATE_POSITION)),
						 1.f };
		Desc.iCurrentCellIndex = 1;
		m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Shot_Effect", L"Prototype_GameObject_Particle_BoxOpen", &Desc);

		Desc.vInitPos = { XMVectorGetX(m_pParentTransform->Get_State(CTransform::STATE_POSITION)),
						 XMVectorGetY(m_pParentTransform->Get_State(CTransform::STATE_POSITION)) + 1.2f,
						 XMVectorGetZ(m_pParentTransform->Get_State(CTransform::STATE_POSITION)) + 0.2f,
						 1.f };
		Desc.iCurrentCellIndex = 2;
		m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Shot_Effect", L"Prototype_GameObject_Particle_BoxOpen", &Desc);


		Desc.vInitPos = { XMVectorGetX(m_pParentTransform->Get_State(CTransform::STATE_POSITION)),
						 XMVectorGetY(m_pParentTransform->Get_State(CTransform::STATE_POSITION)) + 1.3f,
						 XMVectorGetZ(m_pParentTransform->Get_State(CTransform::STATE_POSITION)) + 0.4f,
						 1.f };
		Desc.iCurrentCellIndex = 3;
		m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Shot_Effect", L"Prototype_GameObject_Particle_BoxOpen", &Desc);

	}

	if (2 == m_iStateCurNum)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime >= .8f)
		{
			m_iStateCurNum = 3;

			dynamic_cast<CUI_Etc_BoxItem*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_BoxItemBg")->front())->SetShowYN(true);
			dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front())->MoveStop(true);
			dynamic_cast<CUI_Crosshair*>(GET_LIST(LEVEL_STATIC, L"Layer_Crosshair")->front())->SetFree(true);
		}
	}
}

void CBoxPart::CheckColl()
{
	if (!m_bOpenTextShow)
		return;

	CPlayer* pTargetObject = m_pPlayer;
	if (nullptr == pTargetObject)
		return;

	CCollider* pTargetCollider = pTargetObject->GetPlayerColl();
	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom->Intersect(pTargetCollider))
		dynamic_cast<CUI_Etc_OpenBox*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_OpenBox")->front())->SetShowYN(true);
	else
		dynamic_cast<CUI_Etc_OpenBox*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_OpenBox")->front())->SetShowYN(false);
}

CBoxPart * CBoxPart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoxPart*		pInstance = new CBoxPart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoxPart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBoxPart::Clone(void * pArg)
{
	CBoxPart*		pInstance = new CBoxPart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoxPart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoxPart::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
