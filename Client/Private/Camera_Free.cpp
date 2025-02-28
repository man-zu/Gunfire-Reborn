
#include "Camera_Free.h"

#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs)
	: CCamera( rhs )
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void * pArg)
{
	CAMERA_FREE_DESC*		pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CCamera_Free::PriorityTick(_float fTimeDelta)
{
	if (KEY_HOLD(DIK_F1))
	{
		SetShowYN(false);
	}
	if (KEY_HOLD(DIK_F2))
	{
		SetShowYN(true);
	}

	if (!m_bShowYN)
		return;


	if (m_bBossScene)
		BossScene(fTimeDelta);

	if (KEY_HOLD(DIK_LEFT))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (KEY_HOLD(DIK_RIGHT))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}
	if (KEY_HOLD(DIK_UP))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (KEY_HOLD(DIK_DOWN))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	_long	MouseMove = {};

	if(m_bMouseSensorUse)
	{
		if (MOUSE_MOVE_X(MouseMove))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}
		
		if (MOUSE_MOVE_Y(MouseMove))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
		}
	}

	__super::PriorityTick(fTimeDelta);
}

void CCamera_Free::Tick(_float fTimeDelta)
{

}

void CCamera_Free::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::BossScene(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;

	_vector targetPos = { 55.5f, 2.8f, 74.8f, 1.f};
	if(!m_pTransformCom->Go_DistanceCheck(targetPos, 0.5f))
	{
		if (m_fDelayTime <= 1.1f)
			m_pTransformCom->Go_Target(fTimeDelta, targetPos);
		else
			m_pTransformCom->Go_Target(fTimeDelta * 3.f, targetPos);
	}
	else
	{
	}


	m_pTransformCom->LookAt({ 54.6f, 12.f, 113.6f, 1.f });

	if(m_fDelayTime >= 6.f)
	{
		m_bShowYN = false;
		m_bBossScene = false;
		m_fDelayTime = 0.f;
	}

}

CCamera_Free * CCamera_Free::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Free*		pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CCamera_Free::Clone(void * pArg)
{
	CCamera_Free*		pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Free"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCamera_Free::Free()
{
	__super::Free();

}
