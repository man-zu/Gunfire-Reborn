#include "Player_SR.h"

#include "GameInstance.h"
#include "Model.h"

CPlayer_SR::CPlayer_SR(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CWeapon { pDevice, pContext }
{
}

CPlayer_SR::CPlayer_SR(const CPlayer_SR & rhs)
	: CWeapon( rhs )
{
}

HRESULT CPlayer_SR::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_SR::Initialize(void * pArg)
{
	if (FAILED(__super::InitializeChild(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(1, true);

	/* 최초 위치 설정 */
	m_pPosMove = { 0.6f, -.8f, 1.1f, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pPosMove));
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->SaveStatusWorld();

	m_iBulletMagazine = m_iBulletCurr = 10;
	m_iBulletTot = 100;

	m_fAttack = 100.f;

	return S_OK;
}

void CPlayer_SR::PriorityTick(_float fTimeDelta)
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
		if (m_eState == IDLE)
			m_eState = LOAD1;

		if (m_eState == LOAD1)
		{
			__super::LoadGun();
			m_pModelCom->Set_AnimIndex(2, false);
		}
	}


	if (MOUSE_PUSH(DIMK_RBUTTON))
	{
		m_pModelCom->Set_AnimIndex(4, false);
	}

	if (MOUSE_PUSH(DIMK_LBUTTON))
	{
		if (m_iBulletCurr == 0)
			int a = 0;	// 탄창 빈 소리 들리기
		else
		{
			// 두번 진입하고 있음
			m_iBulletCurr--;
			m_pModelCom->Set_AnimIndex(0, false);

			__super::CreateBullet();
		}
	}

	if (m_eState == IDLE)
	{
		__super::MotionIdle(fTimeDelta, m_pPosMove.y);
	}
}

void CPlayer_SR::Tick(_float fTimeDelta)
{
	if (!m_bUseYN)
		return;

	m_pModelCom->Play_Animation(fTimeDelta);

	if (true == m_pModelCom->Get_Finished())
	{
		m_pModelCom->Set_AnimIndex(1, true);
	}

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_SR::LateTick(_float fTimeDelta)
{
	if (!m_bUseYN)
		return;


	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer_SR::Render()
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

HRESULT CPlayer_SR::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_SR"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_SR::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CPlayer_SR * CPlayer_SR::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_SR*		pInstance = new CPlayer_SR(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_SR"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer_SR::Clone(void * pArg)
{
	CPlayer_SR*		pInstance = new CPlayer_SR(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_SR"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_SR::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
