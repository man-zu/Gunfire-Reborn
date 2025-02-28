#pragma once

#include "Base.h"

/* ȭ�鿡 �׷������� ��ü���� �׸��� ������� ��Ƽ� �����Ѵ�. */
/* �����ϰ� �ִ� ��ü���� Render�Լ��� ȣ���Ѵ�. */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	/* �׸��� ������� ����ü�� �����ߴ�. */
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOWGAMEOBJECT, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderObjects(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);	
	HRESULT Draw();
	void Clear();

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent) {
		m_DebugComponents.emplace_back(pComponent);
		Safe_AddRef(pComponent);
		return S_OK;
	}

#endif

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };	
	list<class CGameObject*>	m_RenderObjects[RENDER_END];

	class CGameInstance*		m_pGameInstance = { nullptr };

	class CVIBuffer_Rect*		m_pVIBuffer = { nullptr };

	/* ����Ÿ�� ������ �н� (0), �������� ���� �н� (1) */
	class CShader*				m_pShader = { nullptr };

	ID3D11DepthStencilView*		m_pLightDepthDSView = { nullptr };

private:
	/* m_WorldMatrix : ȭ���� �� ä��� ���������� ����� Ȱ��Ǳ⶧���� �����صѲ��� .*/
	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

#ifdef _DEBUG
private:
	list<class CComponent*>		m_DebugComponents;
#endif

private:
	HRESULT Render_Priority();
	/* �׸��ڸ� ǥ���ϰ����ϴ� ��ü�鸸 �׸��� �����Լ�. */
	HRESULT Render_ShadowGameObject();
	HRESULT Render_NonBlend();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

private:
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();

	_bool	m_bShow = { true };
#endif



public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END