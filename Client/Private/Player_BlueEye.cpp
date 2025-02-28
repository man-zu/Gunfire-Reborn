#include "Player_BlueEye.h"

#include "GameInstance.h"
#include "Model.h"

CPlayer_BlueEye::CPlayer_BlueEye(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CPlayer_BlueEye::CPlayer_BlueEye(const CPlayer_BlueEye& rhs)
	: CWeapon(rhs)
{
}

HRESULT CPlayer_BlueEye::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_BlueEye::Initialize(void* pArg)
{
	if (FAILED(__super::InitializeChild(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0, true);

	m_pPosMove = { .3f, -.3f, .6f, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pPosMove));
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->SaveStatusWorld();


	m_iBulletMagazine = m_iBulletCurr = 10;
	m_iBulletTot = 400;
	m_fAttack = PLAYER_WEAPON_BLUEEYE;

	return S_OK;
}

void CPlayer_BlueEye::PriorityTick(_float fTimeDelta)
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
			// 4, 5, 6, 2
			if (m_eState == IDLE)
				m_eState = LOAD1;

			if (m_eState == LOAD1)
			{
				if (0 == m_iLoadBulletCnt)
					m_iLoadBulletCnt = m_iBulletMagazine - m_iBulletCurr;

				__super::LoadGun(1);

				if (m_iLoadBulletMotionCnt <= m_iLoadBulletCnt)
				{
					m_pModelCom->Set_AnimIndex(4, false);
					PLAY(L"wpnBasicReload", false);
				}
			}
		}
	}

	if (MOUSE_PUSH(DIMK_LBUTTON))
	{
		if (m_iBulletCurr == 0)
			PLAY(L"wpnNoBullet", false);
		else
		{
			// 두번 진입하고 있음
			m_iBulletCurr--;
			m_pModelCom->Set_AnimIndex(3, false);

			PLAY(L"wpn1", false);
			__super::CreateBlueBullet();
		}
	}

	if (m_eState == IDLE)
	{
		__super::MotionIdle(fTimeDelta, m_pPosMove.y);
	}
}

void CPlayer_BlueEye::Tick(_float fTimeDelta)
{
	if (!m_bUseYN)
		return;

	m_pModelCom->Play_Animation(fTimeDelta);

	MotionControl();

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_BlueEye::LateTick(_float fTimeDelta)
{
	if (!m_bUseYN)
		return;

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer_BlueEye::Render()
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

void CPlayer_BlueEye::MotionControl()
{
	if (true == m_pModelCom->Get_Finished())
	{
		if (m_eState == LOAD1)
		{
			m_eState = LOAD2;
		}
		else if (m_eState == LOAD2)
		{
			__super::LoadGun(1);
			m_iLoadBulletMotionCnt++;
			m_pModelCom->Set_AnimIndex(5, false);  // 부족한 탄 수 만큼

			if (m_iLoadBulletMotionCnt == m_iLoadBulletCnt)
			{
				m_eState = LOAD3;
				m_iLoadBulletMotionCnt = 0;
				m_iLoadBulletCnt = 0;
			}
		}
		else if (m_eState == LOAD3)
		{
			m_pModelCom->Set_AnimIndex(6, false);
			m_eState = LOAD4;
		}
		else if (m_eState == LOAD4)
		{
			PLAY(L"wpnBlueEyeReload2", false);
			m_pModelCom->Set_AnimIndex(2, false);
			m_eState = IDLE;
		}
		else
			m_pModelCom->Set_AnimIndex(0, false);
	}
}

HRESULT CPlayer_BlueEye::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_BlueEye"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_BlueEye::Bind_ShaderResources()
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

CPlayer_BlueEye* CPlayer_BlueEye::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_BlueEye* pInstance = new CPlayer_BlueEye(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_BlueEye"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CPlayer_BlueEye::Clone(void* pArg)
{
	CPlayer_BlueEye* pInstance = new CPlayer_BlueEye(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_BlueEye"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_BlueEye::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
