#include "Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _wstring & strFontFilePath)
{
	if (nullptr != Find_Font(strFontTag))
		return E_FAIL;

	CCustomFont*			pFont = CCustomFont::Create(m_pDevice, m_pContext, strFontFilePath.c_str());
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render(const _wstring & strFontTag, const _wstring & strText, const _float2 & vPosition, _fvector vColor, _float fRotation, _float fScale)
{
	CCustomFont*		pFont = Find_Font(strFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(strText, vPosition, vColor, fRotation, fScale);	
}

CCustomFont * CFont_Manager::Find_Font(const _wstring & strFontTag)
{
	auto	iter = m_Fonts.find(strFontTag);

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;	
}

CFont_Manager * CFont_Manager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	return new CFont_Manager(pDevice, pContext);
}

void CFont_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);
}
