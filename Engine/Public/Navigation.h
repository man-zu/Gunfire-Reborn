#pragma once

#include "Component.h"

/* ���� �� ������ Ÿ�� �ٴ� �� �ֵ��� �ﰢ������ ������ �ִ´�. */
/* �� �׺���̼��� �̿��ϴ� ��ü�� � ���ȿ� �դ������� ���� �ﰢ�� �ε����� �����ϰ� �ִ´�. */

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct
	{
		_int		iCurrentCellIndex;
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _wstring& strNavigationDataFile);
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;
	HRESULT Tick();

public:
	void Update(_fmatrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	_bool	isMove(_fvector vPosition, _int* iOutNeighborIndex, _float3* vOutNormal = nullptr);
	_float	GetHeight(_vector vPos, _int iIdx);

private:
	vector<class CCell*>				m_Cells;
	_int								m_iCurrentIndex = { -1 };
	static _float4x4					m_WorldMatrix;

	

#ifdef _DEBUG
private:
	class CShader*						m_pShader = { nullptr };

public:
	HRESULT AddCell(_float3 vPoint1, _float3 vPoint2);
	HRESULT DeleteCell();
	_int	GetCurrIdx() { return m_iCurrentIndex; }
#endif

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile);
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END