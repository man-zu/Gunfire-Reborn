#include "Input_Device.h"

Engine::CInput_Device::CInput_Device(void)
{

}

HRESULT Engine::CInput_Device::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	m_hWnd = hWnd;

	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// Ű���� ��ü ����
	if (FAILED((m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();


	return S_OK;
}

void Engine::CInput_Device::Update_InputDev()
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);


	HWND hFocus = GetFocus();

	// ���� â�� �ƴ� �ٸ� â�� ư ���
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
				// Ű�� �������
				// ������ ������ �ʾҴٸ� 
				if (NONE == m_eKeyState[i] || AWAY == m_eKeyState[i])
				{
					m_eKeyState[i] = PUSH;
				}
				// ���� �����ӿ� ���� ���¿��ٸ�
				else if (PUSH == m_eKeyState[i])
					m_eKeyState[i] = HOLD;
			}
			else	// Ű�� ������ ���� ���
			{
				// ������ ���� ���¿��ٸ�
				if (PUSH == m_eKeyState[i] || HOLD == m_eKeyState[i])
					m_eKeyState[i] = AWAY;
				// ������ ������ ���� ���¿��ٸ�
				else if (AWAY == m_eKeyState[i])
					m_eKeyState[i] = NONE;
			}
		}
	}

}

void CInput_Device::Update_Mouse_Info()
{
	HWND hFocus = GetFocus();

	// ���� â�� �ƴ� �ٸ� â�� ư ���

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
				// ���콺�� �������
				// ������ ������ �ʾҴٸ� 
				if (NONE == m_eMouseState[i] || AWAY == m_eMouseState[i])
				{
					m_eMouseState[i] = PUSH;
				}
				// ���� �����ӿ� ���� ���¿��ٸ�
				else if (PUSH == m_eMouseState[i] || HOLD == m_eMouseState[i])
					m_eMouseState[i] = HOLD;
			}
			else	// Ű�� ������ ���� ���
			{
				// ������ ���� ���¿��ٸ�
				if (PUSH == m_eMouseState[i] || HOLD == m_eMouseState[i])
					m_eMouseState[i] = AWAY;
				// ������ ������ ���� ���¿��ٸ�
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

