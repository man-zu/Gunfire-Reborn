#include "Player_Body.h"

#include "GameInstance.h"
#include "Player.h"

CPlayer_Body::CPlayer_Body(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext } 
{
}

CPlayer_Body::CPlayer_Body(const CPlayer_Body & rhs)
	: CPartObject(rhs)
{
}

const _float4x4 * CPlayer_Body::Get_BoneMatrixPtr(const _char * pBoneName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pBoneName);	
}

HRESULT CPlayer_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Body::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(1, true);

	_float4 m_pPosMove = { .3f, -1.4f, .2f, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pPosMove));
	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CPlayer_Body::PriorityTick(_float fTimeDelta)
{
	if (KEY_PUSH(DIK_Y))
		m_pTransformCom->LookAtForLandObject({0.f, 1.f, 0.f});
}

void CPlayer_Body::Tick(_float fTimeDelta)
{
	if (KEY_HOLD(DIK_5))
		m_pModelCom->Set_AnimIndex(1, true);
	if (KEY_HOLD(DIK_6))
		m_pModelCom->Set_AnimIndex(2, false);
	if (KEY_HOLD(DIK_7))
		m_pModelCom->Set_AnimIndex(0, false);

	m_pModelCom->Play_Animation(fTimeDelta);

	m_fDelayTime += fTimeDelta;

	if(m_fDelayTime <= 1.5f)
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, fTimeDelta);

	SetUp_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_Body::LateTick(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer_Body::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < 1; i++)
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

HRESULT CPlayer_Body::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_PlayerBody"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Body::Bind_ShaderResources()
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

CPlayer_Body * CPlayer_Body::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Body*		pInstance = new CPlayer_Body(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Body"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer_Body::Clone(void * pArg)
{
	CPlayer_Body*		pInstance = new CPlayer_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_Body"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPlayer_Body::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
