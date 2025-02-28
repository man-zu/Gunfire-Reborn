#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTransform;
class CTexture;
class CShader;
END

BEGIN(Client)

class CBoss_Effect_TargetRing final : public CGameObject
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
	CBoss_Effect_TargetRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Effect_TargetRing(const CBoss_Effect_TargetRing& rhs);
	virtual ~CBoss_Effect_TargetRing() = default;

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
	_float					m_fScale = 1;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBoss_Effect_TargetRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END