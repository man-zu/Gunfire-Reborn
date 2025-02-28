#include "Input_Device.h"

Engine::CInput_Device::CInput_Device(void)
{

}

HRESULT Engine::CInput_Device::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	m_hWnd = hWnd;

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED((m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();


	return S_OK;
}

void Engine::CInput_Device::Update_InputDev()
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);


	HWND hFocus = GetFocus();

	// 게임 창이 아닌 다른 창을 튼 경우
	if (hFocus != m_hWnd)
	{
		for (_uint i = 0; i < 256; ++i)
		{
			if (HOLD == m_eKeyState[i] || PUSH == m_eKeyState[i])
			{
				m_eKeyState[i] = AWAY;
			}

			else if (AWAY == m_eKeyState[i])
			{
				m_eKeyState[i] = NONE;
			}
		}
	}

	if (hFocus == m_hWnd)
	{
		for (_uint i = 0; i < 256; ++i)
		{
			if (m_byKeyState[i] & 0x80)
			{
				// 키가 눌린경우
				// 이전에 눌리지 않았다면 
				if (NONE == m_eKeyState[i] || AWAY == m_eKeyState[i])
				{
					m_eKeyState[i] = PUSH;
				}
				// 이전 프레임에 눌린 상태였다면
				else if (PUSH == m_eKeyState[i])
					m_eKeyState[i] = HOLD;
			}
			else	// 키가 눌리지 않은 경우
			{
				// 이전에 눌린 상태엿다면
				if (PUSH == m_eKeyState[i] || HOLD == m_eKeyState[i])
					m_eKeyState[i] = AWAY;
				// 이전에 눌리지 않은 상태였다면
				else if (AWAY == m_eKeyState[i])
					m_eKeyState[i] = NONE;
			}
		}
	}

}

void CInput_Device::Update_Mouse_Info()
{
	HWND hFocus = GetFocus();

	// 게임 창이 아닌 다른 창을 튼 경우

	if (hFocus != m_hWnd)
	{
		for (_uint i = 0; i < DIMK_END; ++i)
		{
			if (HOLD == m_eMouseState[i] || PUSH == m_eMouseState[i])
			{
				m_eMouseState[i] = AWAY;
			}

			else if (AWAY == m_eMouseState[i])
			{
				m_eMouseState[i] = NONE;
			}
		}
	}
	else if (hFocus == m_hWnd)
	{
		m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_tMouseState);
		for (_uint i = 0; i < DIMK_END; ++i)
		{
			if (m_tMouseState.rgbButtons[i] & 0x80)
			{
				// 마우스가 눌린경우
				// 이전에 눌리지 않았다면 
				if (NONE == m_eMouseState[i] || AWAY == m_eMouseState[i])
				{
					m_eMouseState[i] = PUSH;
				}
				// 이전 프레임에 눌린 상태였다면
				else if (PUSH == m_eMouseState[i] || HOLD == m_eMouseState[i])
					m_eMouseState[i] = HOLD;
			}
			else	// 키가 눌리지 않은 경우
			{
				// 이전에 눌린 상태엿다면
				if (PUSH == m_eMouseState[i] || HOLD == m_eMouseState[i])
					m_eMouseState[i] = AWAY;
				// 이전에 눌리지 않은 상태였다면
				else if (AWAY == m_eMouseState[i] || NONE == m_eMouseState[i])
					m_eMouseState[i] = NONE;
			}
		}
	}
	return;
}

void CInput_Device::Update()
{
	Update_InputDev();
	Update_Mouse_Info();
}

CInput_Device * CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device*		pInstance = new CInput_Device();

	if (FAILED(pInstance->Ready_InputDev(hInst, hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CInput_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

