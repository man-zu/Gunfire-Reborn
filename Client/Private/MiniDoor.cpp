#include "MiniDoor.h"

#include "GameInstance.h"
#include "LoadModel.h"
#include "Player.h"
#include "Level.h"
#include "UI_Etc_NextStage.h"

CMiniDoor::CMiniDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CMiniDoor::CMiniDoor(const CMiniDoor & rhs)
	: CGameObject( rhs )
{
}

HRESULT CMiniDoor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniDoor::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;
	
	m_eTB = static_cast<MINIDOOR_DESC*>(pArg)->eDoorType;

	if (m_eTB == TOP)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MiniDoor_top"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	if (m_eTB == BOTTOM)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MiniDoor_bottom"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 최초 위치 설정 */
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(static_cast<MINIDOOR_DESC*>(pArg)->fTurn));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&static_cast<MINIDOOR_DESC*>(pArg)->vInitPos));

	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	return S_OK;
}

void CMiniDoor::PriorityTick(_float fTimeDelta)
{
	if (0 == m_iMonCnt && GET_CURLEVEL == LEVEL_STAGE1)
		m_iMonCnt = 5;
	else if (0 == m_iMonCnt && GET_CURLEVEL == LEVEL_STAGE2)
		m_iMonCnt = 6;

	_uint iPlayerKillCnt = m_pPlayer->GetKillCnt();

	if (iPlayerKillCnt == m_iMonCnt)
		m_bOpenTextShow = true;

	if (m_bOpenTextShow && KEY_PUSH(DIK_F))
	{
		m_bOpen = true;
		PLAYONLY(L"MiniDoorOpen", false);
		dynamic_cast<CUI_Etc_NextStage*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_NextStage")->front())->SetShowYN(false);
	}

	if (m_bOpen)
	{
		m_fDelayTime += fTimeDelta;
		if(m_fDelayTime <= 3.3f)
		{
			if (m_eTB == TOP)
			{
				m_pTransformCom->Go_Down(-fTimeDelta);
			}
			if (m_eTB == BOTTOM)
			{
				m_pTransformCom->Go_Down(fTimeDelta);
			}
		}
	}
}

void CMiniDoor::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	if (!m_bOpen)		// 문 닫힌 상태
		CheckColl();
}

void CMiniDoor::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMiniDoor::Render()
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

HRESULT CMiniDoor::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingAABBDesc{};

	BoundingAABBDesc.vExtents = _float3(4.f, 5.f, 5.5f);
	BoundingAABBDesc.vCenter = _float3(0.f, BoundingAABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingAABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniDoor::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float3 camPos = (_float3)m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW)->m[3];
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamPos", &camPos, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

void CMiniDoor::CheckColl()
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
		dynamic_cast<CUI_Etc_NextStage*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_NextStage")->front())->SetShowYN(true);
	else
		dynamic_cast<CUI_Etc_NextStage*>(GET_LIST(LEVEL_STATIC, L"Layer_UI_NextStage")->front())->SetShowYN(false);
}

CMiniDoor * CMiniDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniDoor*		pInstance = new CMiniDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMiniDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMiniDoor::Clone(void * pArg)
{
	CMiniDoor*		pInstance = new CMiniDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMiniDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniDoor::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
