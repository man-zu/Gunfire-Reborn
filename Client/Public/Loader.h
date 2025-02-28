#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 2. 로딩 다음 레벨에 필요한 자원을 생성한다.  */
/* 다음 레벨에 대한 자원 로딩 : 멀티스레드 */

BEGIN(Engine)
class CGameInstance;
class CLevel_Manager;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	_uint Loading();
	_bool isFinished() const {
		return m_isFinished;
	}

	void Show_LoadingText() {
		SetWindowText(g_hWnd, m_szLoadingText);
	}

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	HANDLE					m_hThread = {};
	CRITICAL_SECTION		m_CriticalSection = {};
	LEVEL					m_eNextLevelID = { LEVEL_END };

	_tchar					m_szLoadingText[MAX_PATH] = TEXT("");
	_float					m_fLoadingPercent = { 0.0f };
	_bool					m_isFinished = { false };

private:
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	HRESULT Loading_For_Logo_Level();
	HRESULT Loading_For_GamePlay_Level();
	HRESULT Loading_For_Stage1_Level();
	HRESULT Loading_For_Stage2_Level();
	HRESULT Loading_For_Stage1_Boss_Level();

	void Loading_For_Static();
	HRESULT Loading_For_Collider();
	HRESULT Loading_For_Shader();
	HRESULT Loading_For_Player_UI();
	HRESULT Loading_For_Static_Etc();
	HRESULT Loading_For_Sound();

	HRESULT Load_Binary_Files(LEVEL eLevel, const string _path);
	HRESULT Load_Saved_Files(LEVEL eLevel, const string _path);

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END