#pragma once

#include "Component.h"

/* 셰이더에 텍스쳐의 정보를 던져서 셰이더내에서 이 텍스쳐 픽셀의 색을 픽셀에 입히는 용도로 사용하기위한 테긋쳐 집합. */
/* 픽셀의 색을 입히는 용도로 셰이더던지고 쌤플링해온다. */

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{	
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override { return S_OK; }

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex = 0);


private:
	// ID3D11Texture2D -> ID3D11ShaderResourceView, ID3D11RenderTargetView, ID3D11DepthStencilView 
	vector<ID3D11ShaderResourceView*>				m_SRVs;
	_uint											m_iNumTextures = { 0 };

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END