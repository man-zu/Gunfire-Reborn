#include "Map_Stage1_Boss.h"

#include "GameInstance.h"

CMap_Stage1_Boss::CMap_Stage1_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CMap_Stage1_Boss::CMap_Stage1_Boss(const CMap_Stage1_Boss & rhs)
	: CGameObject( rhs )
{
}

HRESULT CMap_Stage1_Boss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap_Stage1_Boss::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, {62.f, -0.8f, 78.f, 1.f});

	return S_OK;
}

void CMap_Stage1_Boss::PriorityTick(_float fTimeDelta)
{
}

void CMap_Stage1_Boss::Tick(_float fTimeDelta)
{
}

void CMap_Stage1_Boss::LateTick(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMap_Stage1_Boss::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;		

		m_pShaderCom->Begin(0);
		
		m_pModelCom->Render(i);
	}	

	return S_OK;
}

HRESULT CMap_Stage1_Boss::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Model_Map_Stage1_Boss"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap_Stage1_Boss::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bDead", &m_bDissolve, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DeltaTime", &m_fDelayTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CMap_Stage1_Boss * CMap_Stage1_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMap_Stage1_Boss*		pInstance = new CMap_Stage1_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMap_Stage1_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMap_Stage1_Boss::Clone(void * pArg)
{
	CMap_Stage1_Boss*		pInstance = new CMap_Stage1_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMap_Stage1_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap_Stage1_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
