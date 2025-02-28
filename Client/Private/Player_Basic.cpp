#include "Player_Basic.h"

#include "GameInstance.h"
#include "Model.h"

CPlayer_Basic::CPlayer_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CPlayer_Basic::CPlayer_Basic(const CPlayer_Basic& rhs)
	: CWeapon(rhs)
{
}

HRESULT CPlayer_Basic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Basic::Initialize(void* pArg)
{
	if (FAILED(__super::InitializeChild(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(1, true);

	/* 최초 위치 설정 */
	m_pPosMove = { .8f, -.9f, 1.3f, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pPosMove));
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->SaveStatusWorld();

	m_iBulletMagazine = m_iBulletCurr = 50;
	m_iBulletTot = 456;

	m_fAttack = PLAYER_WEAPON_BASIC;

	return S_OK;
}

void CPlayer_Basic::PriorityTick(_float fTimeDelta)
{
	if (!m_bUseYN)
		return;

	if (m_eState == CHANGE || m_eState == SKILL)
	{
		__super::MotionWeaponChange(fTimeDelta);
		return;
	}
	if (m_eState == SHOW)
	{
		if (!m_pTransformCom->Go_SavedWorldMatrix(fTimeDelta))
			m_eState = IDLE;
		return;
	}

	if (KEY_PUSH(DIK_R))
	{
		if (m_iBulletCurr != m_iBulletMagazine)
		{
			__super::LoadGun();
			PLAY(L"wpnBasicReload", false);
			m_pModelCom->Set_AnimIndex(2, false);
		}
	}

	_byte mouse = {};
	if (MOUSE_PUSH(DIMK_RBUTTON))
	{
		m_pModelCom->Set_AnimIndex(4, false);
	}

	if (MOUSE_HOLD(DIMK_LBUTTON))
	{
		if (m_iBulletCurr == 0)
			int a = 0;	// 탄창 빈 소리
		else
		{
			m_fDelayTime += fTimeDelta;
			if (m_fDelayTime >= 0.3f)
			{
				m_fDelayTime = 0.f;
				m_iBulletCurr--;
				__super::CreateBullet();
				PLAY(L"wpn0", false);
			}
		}
	}

	if (MOUSE_PUSH(DIMK_LBUTTON))
	{
		if (m_iBulletCurr == 0)
			PLAY(L"wpnNoBullet", false);

		else
		{
			if (m_pModelCom->getCurAnimIdx() != 0)
				m_pModelCom->Set_AnimIndex(0, true);

			PLAY(L"wpn0", false);
			__super::CreateBullet();
		}
	}
	if (MOUSE_AWAY(DIMK_LBUTTON))
	{
		m_pModelCom->Set_AnimIndex(1, true);
	}

	if (m_eState == IDLE)
	{
		__super::MotionIdle(fTimeDelta, m_pPosMove.y);
	}
}

void CPlayer_Basic::Tick(_float fTimeDelta)
{
	if (!m_bUseYN)
		return;

	m_pModelCom->Play_Animation(fTimeDelta);

	MotionControl();

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_Basic::LateTick(_float fTimeDelta)
{
	if (!m_bUseYN)
		return;

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer_Basic::Render()
{
	if (!m_bUseYN)
		return S_OK;

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

void CPlayer_Basic::MotionControl()
{
	if (true == m_pModelCom->Get_Finished())
		m_pModelCom->Set_AnimIndex(1, true);
}

HRESULT CPlayer_Basic::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Basic"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Basic::Bind_ShaderResources()
{
	_float4x4 mat, matInv;
	XMStoreFloat4x4(&mat, XMMatrixIdentity());
	XMStoreFloat4x4(&matInv, XMMatrixIdentity());

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

	_float3 camPos = (_float3)m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW)->m[3];
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamPos", &camPos, sizeof(_float3))))
		return E_FAIL;

	_bool m_bRim = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRedRim", &m_bRim, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bDead", &m_bDissolve, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CPlayer_Basic* CPlayer_Basic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Basic* pInstance = new CPlayer_Basic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Basic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CPlayer_Basic::Clone(void* pArg)
{
	CPlayer_Basic* pInstance = new CPlayer_Basic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_Basic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Basic::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
