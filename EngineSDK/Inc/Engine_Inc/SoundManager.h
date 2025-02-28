#pragma once
#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager(FMOD::System* pFmod_System);
	virtual ~CSound_Manager() = default;


public:
	HRESULT Initialize();
	HRESULT	Add_Sound(const wstring& strTag, const wstring& strPath);
	class CSound* Find_Sound(const wstring& strTag);

	HRESULT Play(const wstring& strSoundTag, _bool bLoop);
	HRESULT	PlayOnly(const wstring& strSoundTag);
	HRESULT PlayToBgm(const wstring& strSoundTag, _bool bLoop);
	HRESULT Stop(const wstring& strSoundTag);
	HRESULT Paused(const wstring& strSoundTag, _bool bPause);
	HRESULT SetVolume(const wstring& strSoundTag, _float fVolume);
	HRESULT SetPosition(const wstring& strSoundTag, _float fPosition);


private:
	FMOD::System* m_pFmod_System = { nullptr };

	map<const wstring, class CSound*> m_Sounds;

public:
	static CSound_Manager* Create(FMOD::System** pFmod_System);
	virtual void Free() override;
};


END