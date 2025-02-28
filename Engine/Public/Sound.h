#pragma once
#include "Base.h"	

BEGIN(Engine)

class ENGINE_DLL CSound final : public CBase
{
private:
	CSound(FMOD::System* pFmod_System);
	virtual ~CSound() = default;


public:
	HRESULT Initialize(const wstring& strPath);

public:
	HRESULT Play(_bool bLoop);
	HRESULT	PlayOnly();
	HRESULT PlayToBgm(_bool bLoop);
	HRESULT Stop();
	HRESULT Paused(_bool bPause);
	HRESULT SetVolume(_int iVolume);
	HRESULT SetPosition(_float fPosition);


private:
	FMOD::System*	m_pFmod_System = { nullptr };
	FMOD::Sound*	m_pSound = { nullptr };
	FMOD::Channel*  m_pChannel = { nullptr };
	_int			m_iVolume = { 100 };
	_float			m_fRealVulume = { 100.f };
	FMOD_BOOL		m_bLoop = { false };


public:
	static CSound* Create(FMOD::System* pFmod_System, const wstring& strPath);
	virtual void Free() override;


};

END