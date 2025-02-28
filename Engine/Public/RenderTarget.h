#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	void Clear();
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const _char* pConstantName);
	HRESULT Copy(ID3D11Texture2D* pResource);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	/* SRV, RTV를 만들어주기위한 사전적인의미를 가진 객체. */
	ID3D11Texture2D*				m_pTexture2D = { nullptr };

	/* 쉐이더에 던져질 수 있는 타입. */
	ID3D11ShaderResourceView*		m_pSRV = { nullptr };

	/* 장치에 바인딩되어 그려질 수 있는 텍스쳐. */
	ID3D11RenderTargetView*			m_pRTV = { nullptr };

	_float4							m_vClearColor = {};

#ifdef _DEBUG
private:
	_float4x4						m_WorldMatrix;
#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	virtual void Free() override;
};

END