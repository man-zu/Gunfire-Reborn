#pragma once

#include "Base.h"
BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;


public :
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void Tick();
	void Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, _float4x4* pWorldMat);
	_float4 Get_PickedPos() const { return m_vPickPos; }


private:
	ID3D11Texture2D*	m_pPickedDepthTexture = { nullptr };
	ID3D11Device*		m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance*		m_pGameInstance = { nullptr };
	
	HWND				m_hWnd;
	_uint				m_iWinSizeX, m_iWinSizeY;
	_float3				m_vRayDir;
	_float3				m_vRayPos;

	_float				m_fViewWidth{}, m_fViewHeight{};
	_float4				m_vPickPos{};

public:
	_bool IntersectTri(const XMVECTOR& orig, const XMVECTOR& dir, const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2, float* t, float* u, float* v);

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;

};

END

