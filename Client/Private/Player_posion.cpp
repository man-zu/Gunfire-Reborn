#include "Player_posion.h"

#include "GameInstance.h"
#include "LoadModel.h"

CPlayer_posion::CPlayer_posion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObject { pDevice, pContext }
{
}

CPlayer_posion::CPlayer_posion(const CPlayer_posion & rhs)
	: CObject( rhs )
{
}

HRESULT CPlayer_posion::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_posion::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMat(static_cast<CTransform::TRANSFORM_DESC*>(pArg)->worldMat);
	m_pTransformCom->Go_Straight(-.1f);
	m_pTransformCom->Go_Left(.01f);

	m_fAttack = 10.f;

	return S_OK;
}

void CPlayer_posion::PriorityTick(_float fTimeDelta)
{
	_float4 fPos;
	_float PosY = 0.f;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fJumpTime += fTimeDelta * 0.8f;

	PosY += fPos.y + (m_fJumpPower * m_fJumpTime) - (m_fGravity * m_fJumpTime * m_fJumpTime * 0.5f);

	if ((m_fJumpPower * m_fJumpTime) < (m_fGravity * m_fJumpTime * m_fJumpTime * 0.5f))
	{
		m_bFall = true;
	}

	if (m_bFall && fPos.y <= -0.25f)
		return;


	m_pTransformCom->Go_Straight(fTimeDelta * .6f);
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	fPos.y = PosY;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&fPos));
}

void CPlayer_posion::Tick(_float fTimeDelta)
{
}

void CPlayer_posion::LateTick(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer_posion::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < 1; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
		//	return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlayer_posion::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */ /* CModel Έν */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Obj_PosionBall",
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_posion::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CPlayer_posion * CPlayer_posion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_posion*		pInstance = new CPlayer_posion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_posion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer_posion::Clone(void * pArg)
{
	CPlayer_posion*		pInstance = new CPlayer_posion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_posion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_posion::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
