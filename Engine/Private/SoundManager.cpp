#include "SoundManager.h"
#include "Sound.h"

CSound_Manager::CSound_Manager(FMOD::System* pFmod_System)
	: m_pFmod_System{ pFmod_System }
{
}

HRESULT CSound_Manager::Initialize()
{
	return S_OK;
}

HRESULT CSound_Manager::Add_Sound(const wstring& strTag, const wstring& strPath)
{
	CSound* pSound = Find_Sound(strTag);
	if (nullptr != pSound || nullptr == m_pFmod_System)
	{
		MSG_BOX(L"CSound_Manager - Add_Sound FAILED");
		return E_FAIL;
	}

	pSound = CSound::Create(m_pFmod_System, strPath);
	if (nullptr == pSound)
		return E_FAIL;


	m_Sounds.emplace(strTag, pSound);
	return S_OK;
}

CSound* CSound_Manager::Find_Sound(const wstring& strTag)
{
	auto iter = m_Sounds.find(strTag);
	if (m_Sounds.end() == iter)
	{
		return nullptr;
	}
	return iter->second;
}

HRESULT CSound_Manager::Play(const wstring& strSoundTag, _bool bLoop)
{
	auto iter = m_Sounds.find(strSoundTag);
	if (m_Sounds.end() == iter)
	{
		return E_FAIL;
	}

	return iter->second->Play(bLoop);
}

HRESULT CSound_Manager::PlayOnly(const wstring& strSoundTag)
{
	auto iter = m_Sounds.find(strSoundTag);
	if (m_Sounds.end() == iter)
	{
		return E_FAIL;
	}

	return iter->second->PlayOnly();
}

HRESULT CSound_Manager::PlayToBgm(const wstring& strSoundTag, _bool bLoop)
{
	auto iter = m_Sounds.find(strSoundTag);
	if (m_Sounds.end() == iter)
	{
		return E_FAIL;
	}
	return iter->second->PlayToBgm(bLoop);
}

HRESULT CSound_Manager::Stop(const wstring& strSoundTag)
{
	auto iter = m_Sounds.find(strSoundTag);
	if (m_Sounds.end() == iter)
	{
		return E_FAIL;
	}

	return iter->second->Stop();
}

HRESULT CSound_Manager::Paused(const wstring& strSoundTag, _bool bPause)
{
	auto iter = m_Sounds.find(strSoundTag);
	if (m_Sounds.end() == iter)
	{
		return E_FAIL;
	}

	return iter->second->Paused(bPause);
}

HRESULT CSound_Manager::SetVolume(const wstring& strSoundTag, _float fVolume)
{
	auto iter = m_Sounds.find(strSoundTag);
	if (m_Sounds.end() == iter)
	{
		return E_FAIL;
	}
	return iter->second->SetVolume(fVolume);
}

HRESULT CSound_Manager::SetPosition(const wstring& strSoundTag, _float fPosition)
{
	auto iter = m_Sounds.find(strSoundTag);
	if (m_Sounds.end() == iter)
	{
		return E_FAIL;
	}
	return iter->second->SetPosition(fPosition);
}

CSound_Manager* CSound_Manager::Create(FMOD::System** pFmod_System)
{
	CSound_Manager* pInstance = new CSound_Manager(*pFmod_System);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(L"Failed To Created CSound_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Sounds)
	{
		Safe_Release(Pair.second);
	}
	m_Sounds.clear();
	m_pFmod_System->release();
}
