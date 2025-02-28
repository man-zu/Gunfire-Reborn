#include "Terrain.h"

#include "GameInstance.h"


CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject( rhs )
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	m_eLevel = reinterpret_cast<TERRAIN_DESC*>(pArg)->eLevel;

	CTransform::TRANSFORM_DESC			TransformDesc{};
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&TransformDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::PriorityTick(_float fTimeDelta)
{
}

void CTerrain::Tick(_float fTimeDelta)
{
	m_pNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());


	if (KEY_PUSH(DIK_COMMA))
	{
		m_bShowTexture = !m_bShowTexture;
	}
	//if(KEY_PUSH(DIK_Z))
	//{
	//	_float3 pPos = { 0.f, 0.f, 0.f };
	//	_bool pickingResult = m_pVIBufferCom->Picking(m_pTransformCom, &pPos);
	//	
	//	if(pickingResult)
	//	{
	//		std::cout << pPos.x << ", " << pPos.y << ", " << pPos.z << endl;
	//		std::cout << "-----------------------------------------------------------" << endl;	// 월드좌표
	//	}
	//}
}

void CTerrain::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	/* 렌더러클래스에 디버그용 렌더그룹에 추가해주고 디퍼드단계가 아닌 위치에서 컴포넌트들을 그릴 수 있도록 하겠다. */
	//m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CTerrain::Render()
{
	if(m_bShowTexture)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if(m_eLevel == LEVEL_STAGE1)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Prototype_Component_Navigation_Stage1"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
			return E_FAIL;
	}
	if(m_eLevel == LEVEL_STAGE1_BOSS)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_STAGE1_BOSS, TEXT("Prototype_Component_Navigation_StageBoss"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	/* 빛 연산. */
	/* (L.Diffuse * M.Diffuse) * Shade */
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;




	return S_OK;
}

void CTerrain::GetPickPos(_float3* vPickPos)
{
	if (KEY_PUSH(DIK_Z))
	{
		_bool pickingResult = m_pVIBufferCom->Picking(m_pTransformCom, vPickPos);

		if (pickingResult)
		{
			std::cout << " [terrain.cpp] vPickPos : " << vPickPos->x << ", " << vPickPos->y << ", " << vPickPos->z << endl;
			std::cout << "-----------------------------------------------------------" << endl;	// 월드좌표
		}
	}
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pNavigationCom);
}
