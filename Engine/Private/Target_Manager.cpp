#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

CTarget_Manager::CTarget_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{


	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring & strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4 & vClearColor)
{
	if (nullptr != Find_RenderTarget(strRenderTargetTag))
		return E_FAIL;

	CRenderTarget*	pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixel, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring & strMRTTag, const _wstring & strRenderTargetTag)
{
	/* 멀티 렌더 타겟에 추가해주기위한 렌더 타겟을 찾았다. */
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	/* 위에서 찾아놓은 렌더타겟을 추가해주기위한 멀티렌더 타겟을 찾자. */
	list<CRenderTarget*>*	pMRTList = Find_MRT(strMRTTag);

	/* 추가하려 했던 멀티렌더 타겟 그룹이 없었다. */
	/* 새롭게 그룹을 생성해주자. */
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.emplace_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else 	/* 추가하려 했던 멀티렌더 타겟 그룹이 있었다. */
		pMRTList->emplace_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring & strMRTTag, ID3D11DepthStencilView* pDSV)
{
	ID3D11ShaderResourceView*			pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
		nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);


	list<CRenderTarget*>*	pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;	

	/* 장치에 이미 바인되어있었던 렌더타겟 하나만 가져올게.(1개, 백버퍼) */
	/* 깊이 버퍼도 가져와서 저장해두자. */
	m_pContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pDSV);

	ID3D11RenderTargetView*		pMRTs[8] = {};

	_uint		iNumRenderTargets = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pMRTs[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}

	if (nullptr != pDSV)
		m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	/* 장치에 렌더타겟을 바인딩한다. */
	/* == 이 렌더타겟에 뭔가르 그리겠다. */ 
	/* == 그릴때 마다 깊이 테스트가 필요하다. */
	/* == 렌더타겟과 깊이버퍼를 함께 바인딩해줘.  */	
	m_pContext->OMSetRenderTargets(iNumRenderTargets, pMRTs, nullptr == pDSV ? m_pDSV : pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pOldRTV, m_pDSV);

	Safe_Release(m_pOldRTV);
	Safe_Release(m_pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_RT_SRV(const _wstring & strRenderTargetTag, CShader * pShader, const _char * pConstantName)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResourceView(pShader, pConstantName);	
}

HRESULT CTarget_Manager::Copy_Resource(const _wstring & strRenderTargetTag, ID3D11Texture2D * pResource)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;
	return pRenderTarget->Copy(pResource);	
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(const _wstring & strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);	
}

HRESULT CTarget_Manager::Render_Debug(const _wstring & strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	if (FAILED(pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Render_Debug(pShader, pVIBuffer);

	return S_OK;
}

#endif

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _wstring & strRenderTargetTag)
{
	auto	iter = m_RenderTargets.find(strRenderTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;	
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring & strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;	
}

CTarget_Manager * CTarget_Manager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTarget_Manager*		pInstance = new CTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CTarget_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTarget_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
