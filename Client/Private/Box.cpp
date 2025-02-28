#include "Box.h"

#include "GameInstance.h"
#include "Player.h"

CBox::CBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CBox::CBox(const CBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBox::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC trans;
	trans.fSpeedPerSec = 1.f;
	trans.fRotationPerSec = 1.f;

 	if (FAILED(__super::Initialize(&trans)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPosMove = static_cast<CTransform::TRANSFORM_DESC*>(pArg)->vInitPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pPosMove));
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(static_cast<CTransform::TRANSFORM_DESC*>(pArg)->fTurn));

	if (FAILED(Ready_PartObjects(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBox::PriorityTick(_float fTimeDelta)
{
	for (auto& pPartObj : m_Parts)
		pPartObj->PriorityTick(fTimeDelta);
}

void CBox::Tick(_float fTimeDelta)
{
	for (auto& pPartObj : m_Parts)
		pPartObj->Tick(fTimeDelta);
}

void CBox::LateTick(_float fTimeDelta)
{
	for (auto& pPartObj : m_Parts)
		pPartObj->LateTick(fTimeDelta);
	
	m_pGameInstance->Add_RenderObjects(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBox::Render()
{
	return S_OK;
}

HRESULT CBox::Ready_PartObjects(void* pArg)
{
	/* Monster Model */
	CPartObject::PARTOBJ_DESC		BodyDesc{};
	
	BodyDesc.pParentTransform = m_pTransformCom;
	BodyDesc.fSpeedPerSec = 1.5f;
	BodyDesc.fRotationPerSec = 1.5f;
	
	 m_pBody = dynamic_cast<CBoxPart*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_BoxPart"), &BodyDesc));
	if (nullptr == m_pBody)
		return E_FAIL;
	m_Parts.emplace_back(m_pBody);

	return S_OK;
}

HRESULT CBox::Ready_Components()
{
	return S_OK;
}

CBox* CBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBox* pInstance = new CBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBox::Clone(void* pArg)
{
	CBox* pInstance = new CBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBox::Free()
{
	__super::Free();

	for (auto& pPartObj : m_Parts)
		Safe_Release(pPartObj);
}
