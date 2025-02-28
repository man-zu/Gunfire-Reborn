#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent{ pDevice, pContext }
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent{ rhs }
	, m_iNumTextures { rhs.m_iNumTextures }
	, m_SRVs{ rhs.m_SRVs }
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const _wstring& strTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	for (size_t i = 0; i < iNumTextures; i++)
	{
		ID3D11ShaderResourceView*		pSRV = { nullptr };

		_tchar		szTextureFilePath[MAX_PATH] = TEXT("");
		_tchar		szExt[MAX_PATH] = TEXT("");

		wsprintf(szTextureFilePath, strTextureFilePath.c_str(), i);
				
		/* D:\Á¤ÀÇÈÆ\143\3d\Framework\Engine\ThirdPartyLib\Temp.jpg */
		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		if (false == lstrcmp(szExt, TEXT(".dds")))
		{	
			if (FAILED(CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV)))
				return E_FAIL;			
		}
		else if (false == lstrcmp(szExt, TEXT(".tga")))
		{
			return E_FAIL;
		}
		else
		{
			if (FAILED(CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV)))
				return E_FAIL;
		}

		m_SRVs.emplace_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader * pShader, const _char * pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);
	
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _wstring & strTextureFilePath, _uint iNumTextures)
{
	CTexture*		pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTextures)))
	{
		MSG_BOX(TEXT("Faile^d to Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CTexture::Clone(void * pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();
}
