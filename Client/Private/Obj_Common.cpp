#include "Obj_Common.h"

#include "GameInstance.h"
#include "LoadModel.h"

CObj_Common::CObj_Common(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObject { pDevice, pContext }
{
}

CObj_Common::CObj_Common(const CObj_Common & rhs)
	: CObject( rhs )
{
}

HRESULT CObj_Common::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CObj_Common::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	OBJDESC* pParam = static_cast<OBJDESC*>(pArg);

	wstring sComponentName = L"";
	sComponentName.assign(pParam->sComponentName.begin(), pParam->sComponentName.end());

	if (FAILED(Ready_Components(sComponentName)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { pParam->fPosX, pParam->fPosY , pParam->fPosZ , 1.f });
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(pParam->fRotY));

	return S_OK;
}

void CObj_Common::PriorityTick(_float fTimeDelta)
{
}

void CObj_Common::Tick(_float fTimeDelta)
{
}

void CObj_Common::LateTick(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CObj_Common::Render()
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

HRESULT CObj_Common::Ready_Components(wstring sComponentName)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */ /* CModel Έν */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, sComponentName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CObj_Common::Bind_ShaderResources()
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

CObj_Common * CObj_Common::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CObj_Common*		pInstance = new CObj_Common(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CObj_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CObj_Common::Clone(void * pArg)
{
	CObj_Common*		pInstance = new CObj_Common(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CObj_Common"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObj_Common::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
