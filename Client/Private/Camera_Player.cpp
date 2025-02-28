
#include "Camera_Player.h"

#include "GameInstance.h"
#include "Player.h"
#include "Level.h"
#include "Camera_Free.h"
#include "UI_Crosshair.h"

CCamera_Player::CCamera_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CCamera_Player::CCamera_Player(const CCamera_Player & rhs)
	: CPartObject( rhs )
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Player::Initialize(void * pArg)
{
	PLAYER_CAMERA_DESC*		pDesc = static_cast<PLAYER_CAMERA_DESC*>(pArg);
	
	m_tPlayerCam.fMouseSensor = pDesc->fMouseSensor;
	m_tPlayerCam.fFovy = pDesc->fFovy;
	m_tPlayerCam.fAspect = pDesc->fAspect;
	m_tPlayerCam.fNear = pDesc->fNear;
	m_tPlayerCam.fFar = pDesc->fFar;
	m_tPlayerCam.m_pPlayerNavigationCom = pDesc->m_pPlayerNavigationCom;

	Safe_AddRef(m_tPlayerCam.m_pPlayerNavigationCom);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pParentTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pParentTransform->LookAtForLandObject(XMLoadFloat4(&pDesc->vAt));

	m_pPosMove = { 0.f, -5.f, 0.f, 1.f };

	m_bShowYN = true;

	m_pPlayer = pDesc->m_pPlayer;

	return S_OK;
}

void CCamera_Player::PriorityTick(_float fTimeDelta)
{
	if (KEY_HOLD(DIK_F1))
	{
		m_bShowYN = true;
	}
	if (KEY_HOLD(DIK_F2))
	{
		m_bShowYN = false;
	}

	//Layer_MainCamera
	if(!m_bShowYN && GET_CURLEVEL == LEVEL_STAGE1_BOSS)
	{
		if(!dynamic_cast<CCamera_Free*>(GET_LIST(LEVEL_STAGE1_BOSS, L"Layer_MainCamera")->front())->IsBossScene())
		{
			m_bShowYN = true;
			list<CGameObject*>* CrosshairUi = GET_LIST(LEVEL_STATIC, L"Layer_Crosshair");
			dynamic_cast<CUI_Crosshair*>(CrosshairUi->front())->SetHide(false);
		}

	}

	_long	MouseMove = {};

	if (KEY_NONE(DIK_W) && KEY_NONE(DIK_S) && KEY_NONE(DIK_A) && KEY_NONE(DIK_D) && KEY_NONE(DIK_SPACE))
	{
		if(m_pPlayer->GetPlayerState() != CPlayer::PLAYERSTATE::DIE && m_pPlayer->GetPlayerState() != CPlayer::PLAYERSTATE::GODMODE)
			m_pPlayer->SetPlayerState(CPlayer::PLAYERSTATE::STOP);
	}
	else
		m_pPlayer->SetPlayerState(CPlayer::PLAYERSTATE::MOVE);
	
	if(!m_bMoveStop)	// if boxopen? PlayerMoveStop
	{
		if (MOUSE_MOVE_X(MouseMove))
		{
			m_pParentTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_tPlayerCam.fMouseSensor);
		}

		if (MOUSE_MOVE_Y(MouseMove))
		{
			_vector vRight = m_pParentTransform->Get_State(CTransform::STATE_RIGHT);

			m_pParentTransform->Turn(vRight, MouseMove * fTimeDelta * 0.1f);
		}
	}

	if (m_pPlayer->GetPlayerState() == CPlayer::PLAYERSTATE::DIE)
	{

	}

	if (!m_bShowYN)
		return;

	///* 카메라 월드행렬의 역행렬을 뷰행렬로서 파이프라인에 저장해준다. */
	//m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, m_pParentTransform->Get_WorldMatrix()));
	//m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_tPlayerCam.fFovy, m_tPlayerCam.fAspect, m_tPlayerCam.fNear, m_tPlayerCam.fFar));
	//
	//m_pColliderCom->Update(m_pParentTransform->Get_WorldMatrix());

}

void CCamera_Player::Tick(_float fTimeDelta)
{
	if (!m_bShowYN)
		return;

	/* 카메라 월드행렬의 역행렬을 뷰행렬로서 파이프라인에 저장해준다. */
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, m_pParentTransform->Get_WorldMatrix()));
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_tPlayerCam.fFovy, m_tPlayerCam.fAspect, m_tPlayerCam.fNear, m_tPlayerCam.fFar));

	m_pColliderCom->Update(m_pParentTransform->Get_WorldMatrix());
}

void CCamera_Player::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);



#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CCamera_Player::Render()
{
	return S_OK;
}

HRESULT CCamera_Player::Ready_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC	BoundingDesc{};

	BoundingDesc.vExtents = _float3(0.3f, 0.3f, 0.3f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

CCamera_Player * CCamera_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Player*		pInstance = new CCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CCamera_Player::Clone(void * pArg)
{
	CCamera_Player*		pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCamera_Player::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_tPlayerCam.m_pPlayerNavigationCom);
}
