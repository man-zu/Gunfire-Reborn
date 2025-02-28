#include "Sound.h"

CSound::CSound(FMOD::System* pFmod_System)
	:m_pFmod_System{pFmod_System}
{
}

HRESULT CSound::Initialize(const wstring& strPath)
{
	string strTmp = string(strPath.begin(), strPath.end());

	FMOD_RESULT result = m_pFmod_System->createSound(strTmp.c_str(), FMOD_DEFAULT, NULL, &m_pSound);
	if (FMOD_OK != result)
	{
		m_pSound->release();
		m_pSound = nullptr;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSound::Play(_bool bLoop)
{
	if (nullptr == m_pSound || nullptr == m_pFmod_System)
		return E_FAIL;

	FMOD_RESULT Result;

	Result = m_pFmod_System->playSound(m_pSound, 0, false, &m_pChannel);

	if (Result != FMOD_OK)
	{
		MSG_BOX(L"CSound - playSound Failed");
		return E_FAIL;
	}

	if (true == bLoop)
	{
		m_pChannel->setMode(FMOD_LOOP_NORMAL);
		SetVolume(m_iVolume);
	}
	else
	{  
		m_pChannel->setMode(FMOD_LOOP_OFF);
		SetVolume(m_iVolume);
	}

	return S_OK;
}

HRESULT CSound::PlayOnly()
{
	if (nullptr == m_pSound || nullptr == m_pFmod_System)
		return E_FAIL;
	FMOD_RESULT Result;

	_bool bIsPlaying = { false };
	m_pChannel->isPlaying(&bIsPlaying);

	if (false == bIsPlaying)
	{
		Result = m_pFmod_System->playSound(m_pSound, nullptr, false, &m_pChannel);
		if (Result != FMOD_OK)
		{
			MSG_BOX(L"CSound - playSound Failed");
			return E_FAIL;
		}
		m_pChannel->setMode(FMOD_LOOP_OFF);
	}
	return S_OK;
}



HRESULT CSound::PlayToBgm(_bool bLoop)
{
	if (nullptr == m_pSound || nullptr == m_pFmod_System)
		return E_FAIL;
	FMOD_RESULT Result;

	Result = m_pFmod_System->playSound(m_pSound, nullptr, false, &m_pChannel);
	if (Result != FMOD_OK)
	{
		MSG_BOX(L"CSound - playSound Failed");
		return E_FAIL;
	}
	if (true == bLoop)
	{
		m_pChannel->setMode(FMOD_LOOP_NORMAL);
		SetVolume(m_fRealVulume);
	}
	else
	{
		m_pChannel->setMode(FMOD_LOOP_OFF);
	}
	
	return S_OK;
}

HRESULT CSound::Stop()
{
	if (nullptr == m_pSound || nullptr == m_pFmod_System || nullptr == m_pChannel)
		return E_FAIL;

	m_pChannel->stop();

	return S_OK;
}

HRESULT CSound::Paused(_bool bPause)
{
	if (nullptr == m_pSound || nullptr == m_pFmod_System || nullptr == m_pChannel)
		return E_FAIL;

	m_pChannel->setPaused(bPause);

	return S_OK;
}

HRESULT CSound::SetVolume(_int iVolume)
{
	if (nullptr == m_pSound || nullptr == m_pFmod_System || nullptr == m_pChannel)
		return E_FAIL;

	m_fRealVulume = iVolume *0.01f;

	m_pChannel->setPaused(true);
	m_pChannel->setVolume(m_fRealVulume);
	m_pChannel->setPaused(false);

	return S_OK;
}

HRESULT CSound::SetPosition(_float fPosition)
{
	if (nullptr == m_pSound || nullptr == m_pFmod_System || nullptr == m_pChannel)
		return E_FAIL;

	_uint iLen;
	m_pSound->getLength(&iLen, FMOD_TIMEUNIT_MS);
	fPosition = (_float)iLen * fPosition / 100.f;

	m_pChannel->setPosition((UINT)fPosition, FMOD_TIMEUNIT_MS);

	return S_OK;
}


CSound* CSound::Create(FMOD::System* pFmod_System, const wstring& strPath)
{
	CSound* pInstance = new CSound(pFmod_System);

	if (FAILED(pInstance->Initialize(strPath)))
	{
		MSG_BOX(L"Failed To Created CSound_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound::Free()
{
	__super::Free();
	m_pSound->release();
	//m_pFmod_System->release();

}
