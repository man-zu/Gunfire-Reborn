#include "Player_Skill.h"

#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

CPlayer_Skill::CPlayer_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CPlayer_Skill::CPlayer_Skill(const CPlayer_Skill& rhs)
	: CWeapon( rhs )
{
}

HRESULT CPlayer_Skill::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Skill::Initialize(void* pArg)
{
	if (FAILED(__super::InitializeChild(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0, false);

	/* 최초 위치 설정 */
	m_pPosMove = { -18.4f, 4.5f, 32.f, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pPosMove));
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->SaveStatusWorld();
	m_fAttack = PLAYER_WEAPON_BASIC;

	return S_OK;
}

void CPlayer_Skill::PriorityTick(_float fTimeDelta)
{
	if(m_pPlayer == nullptr)
		m_pPlayer = dynamic_cast<CPlayer*>(GET_LIST(LEVEL_STATIC, L"Layer_Player")->front());

	if (KEY_HOLD(DIK_Q))		// chain
		m_pModelCom->Set_AnimIndex(2, false);
	if (KEY_HOLD(DIK_E))		// posion
		m_pModelCom->Set_AnimIndex(5, false);

	return;
}

void CPlayer_Skill::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_Skill::LateTick(_float fTimeDelta)
{
	//if(m_pPlayer->GetHp() > 0.f)
	//	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer_Skill::Render()
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

HRESULT CPlayer_Skill::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Skill"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Skill::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CPlayer_Skill* CPlayer_Skill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Skill* pInstance = new CPlayer_Skill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Skill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CPlayer_Skill::Clone(void* pArg)
{
	CPlayer_Skill* pInstance = new CPlayer_Skill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_Skill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Skill::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
