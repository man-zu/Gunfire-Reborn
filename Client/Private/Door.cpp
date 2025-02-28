#include "Door.h"

#include "GameInstance.h"
#include "LoadModel.h"
#include "Player.h"
#include "Level.h"
#include "Camera_Player.h"
#include "Camera_Free.h"
#include "UI_Crosshair.h"

CDoor::CDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CDoor::CDoor(const CDoor & rhs)
	: CGameObject( rhs )
{
}

HRESULT CDoor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoor::Initialize(void * pArg)
{
	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eLF = static_cast<DOOR_DESC*>(pArg)->eDoorType;
	m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	if(m_eLF == RIGHT)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&static_cast<DOOR_DESC*>(pArg)->vInitPos));
	}
	if (m_eLF == LEFT)
	{
		m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(180));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&static_cast<DOOR_DESC*>(pArg)->vInitPos));
	}
	float fScale = static_cast<DOOR_DESC*>(pArg)->fScale;
	float fTurn = static_cast<DOOR_DESC*>(pArg)->fTurn;
	m_pTransformCom->Set_Scale({1.19f * fScale, 1.24f * fScale, 2.5f * fScale });

	if(fTurn != 0.f)
		m_pTransformCom->Rotation({0.f, 1.f, 0.f}, XMConvertToRadians(fTurn));

	return S_OK;
}

void CDoor::PriorityTick(_float fTimeDelta)
{
	_vector curPlayerPos = m_pPlayer->GetPlayerPos();
	if ((XMVectorGetZ(curPlayerPos) >= 33.5f && XMVectorGetZ(curPlayerPos) <= 34.f))
	{
		m_bTurn = true;
		m_bOpen = true;
		PLAYONLY(L"BossDoor", false);
		PLAYONLY(L"BossIntro", false);

		CCamera_Player* m_pCam = dynamic_cast<CPlayer*>(GET_OBJ(LEVEL_STATIC, L"Layer_Player", 0))->GetPlayerCam();
		m_pCam->SetShowYN(false);
		dynamic_cast<CCamera_Free*>(GET_OBJ(LEVEL_STAGE1_BOSS, L"Layer_MainCamera", 0))->BossSceneYN(true, m_pCam->CurPosMatrix());
		list<CGameObject*>* CrosshairUi = GET_LIST(LEVEL_STATIC, L"Layer_Crosshair");
		dynamic_cast<CUI_Crosshair*>(CrosshairUi->front())->SetHide(true);
	}

	if (!m_bTurn)
		return;

	m_fDelayTime += fTimeDelta;
	m_pPlayer->GoStraight(fTimeDelta * .55f);

	if (m_fDelayTime >= 2.3f)
	{
		m_fDelayTime = 0.f;
		m_bTurn = false;
		m_pPlayer->SetMoveSelf();
	}

	if (m_bTurn && m_eLF == RIGHT)
	{
		m_pTransformCom->Turn({0.f, 1.f, 0.f}, fTimeDelta * 0.7f);
	}
	if (m_bTurn && m_eLF == LEFT)
	{
		m_pTransformCom->Turn({0.f, -1.f, 0.f}, fTimeDelta * 0.7f);
	}
}

void CDoor::Tick(_float fTimeDelta)
{
}

void CDoor::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CDoor::Render()
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

HRESULT CDoor::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Door"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoor::Bind_ShaderResources()
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

CDoor * CDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDoor*		pInstance = new CDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CDoor::Clone(void * pArg)
{
	CDoor*		pInstance = new CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoor::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
