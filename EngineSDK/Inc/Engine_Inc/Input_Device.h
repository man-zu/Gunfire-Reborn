#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
public:
	using INPUT_STATE = enum Input_State
	{
		NONE, PUSH, HOLD, AWAY
	};

private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;
	
public:
	INPUT_STATE	Get_DIKeyState(_ubyte byKeyID)	{
		return m_eKeyState[byKeyID]; 
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) 	{ 	
		return m_tMouseState.rgbButtons[eMouse]; 	
	}

	//_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)	
	//{	
	//	return *(((_long*)&m_tMouseState) + eMouseState);	
	//}
	INPUT_STATE Get_DIMKeyState(MOUSEKEYSTATE eMouseKeyID) { return m_eMouseState[eMouseKeyID]; }
	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID) { return ((_long*)&m_tMouseState)[eMouseMoveID]; }

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev();
	void	Update_Mouse_Info();
	void	Update();

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };
	
	HWND					m_hWnd;
	INPUT_STATE				m_eKeyState[256] = {};
	_byte					m_byKeyState[256] = {};		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	INPUT_STATE				m_eMouseState[DIMK_END] = { NONE };
	DIMOUSESTATE			m_tMouseState = {};

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
END

