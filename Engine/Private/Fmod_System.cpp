#include "Fmod_System.h"

CFmod_System::CFmod_System()
{
}

HRESULT CFmod_System::Initialize()
{
	FMOD_RESULT fmResult = { FMOD_OK };

	fmResult = FMOD::System_Create(&m_pFmod_System);

	if (FMOD_OK != fmResult)
	{
		MSG_BOX(L"Failed To Create Fmod System");
		return E_FAIL;
	}

	fmResult = m_pFmod_System->init(MAX_CHANNEL, FMOD_INIT_NORMAL, nullptr);
	if (fmResult != FMOD_OK)
	{
		MSG_BOX(L"Failed To Initialize Fmod System");
		return E_FAIL;
	}

	return S_OK;
}

void CFmod_System::FmodUpdate()
{
	m_pFmod_System->update();
}

CFmod_System* CFmod_System::Create()
{
	CFmod_System* pInstance = new CFmod_System();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"Failed To Created CFmod_System");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFmod_System::Free()
{
	__super::Free();
	m_pFmod_System->close();
	m_pFmod_System->release();
}
