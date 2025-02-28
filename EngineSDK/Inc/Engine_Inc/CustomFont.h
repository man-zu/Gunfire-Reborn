#pragma once

#include "Base.h"

/* 폰트 파일하나를 빌드한 객체 */
BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	HRESULT Render(const _wstring & strText, const _float2 & vPosition, _fvector vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float rotation = 0.f, _float fScale = 1.f);
	

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	SpriteBatch*				m_pBatch = { nullptr };
	SpriteFont*					m_pFont = { nullptr };


public:
	static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual void Free() override;
};

END