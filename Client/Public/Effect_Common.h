#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
END

BEGIN(Client)

class CEffect_Common final : public CGameObject
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_float4x4			pWorldTransform = { };
		_uint				iImgCnt = 0;
		_float				fSpeed = 1.f;
		_float3				vScale = {0.f, 0.f, 0.f};
		_float4				MoveAmount = { 0.f, 0.f, 0.f, 0.f};
		_bool				bIsLoop = { false };
		_uint				iLoopfTime = 0;
		wstring				sTextureTag = L"";
		_uint				iRotRadian = 0;
	}EFFECT_DESC;

private:
	CEffect_Common(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Common(const CEffect_Common& rhs);
	virtual ~CEffect_Common() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

	_float					m_fSpeed = { 1.f };
	_float					m_fFrame = { 0.f };
	_uint					m_iImgCnt = 0;
	_bool					m_bIsLoop = { false };
	_uint					m_iLoopTime = 0;
	_uint					m_iLoopCount = 0;

private:
	HRESULT Ready_Components(wstring sTextureTag);
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Common* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END