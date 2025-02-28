#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Object.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager);
private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
	void Tick();
	HRESULT Render();

private:
	void GuizmoBeginSet();

private:
	void UseGuizmo(_float4* _pos);
	
	/* Object Tab */
	void LoadObjDatas();
	void SaveObjDatas();
	void LoadObjItem();
	void SaveObjItem(_uint idx, _float4* _pos, _float _rotY);
	void RemoveData(_uint idx);
	void PickAdd(_uint idx);

	/* json */
	string ParsingJSON();
	HRESULT SaveJSON(string _sJsonData, string _filePath);
	HRESULT LoadJSON();

	/* Navigation Tab */
	HRESULT LoadNavi();
	HRESULT SaveNavi();
	void ReturnNavi();
	void DrawPoints();
	void HideSavedNavi(_bool _b);

	void test();

	/* 공통으로 빼고 싶다 */
	void Rounds(_float& self, _int _iRound);
	void SetPointsClockWise(_float3* pPoints);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	CNavigation*				m_pNavigationCom = { nullptr };
	
	_bool						m_bMoseMove = { true };
	GIZMODESC					m_tGizmoDesc = {};
	class CGameObject*			m_pSelectObj = { nullptr };
	vector<CObject::OBJDESC*>	m_tOBJDatas;

	_float3						m_fPoint[3] = {};
	_int						m_iPointCnt = { 0 };
	vector<_float3>				m_vNaviDatas;

	static _int					iCntIdx;

public:
	virtual void Free() override;

private:
	class CGameInstance* m_pGameInstance = { nullptr };
};

END