#pragma once

#include "Component.h"

/* ���̴��� �ؽ����� ������ ������ ���̴������� �� �ؽ��� �ȼ��� ���� �ȼ��� ������ �뵵�� ����ϱ����� �ױ��� ����. */
/* �ȼ��� ���� ������ �뵵�� ���̴������� ���ø��ؿ´�. */

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