#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CFmod_System : public CBase
{

private:
	CFmod_System();
	virtual ~CFmod_System() = default;

public:
	HRESULT Initialize();
	void	FmodUpdate();
	FMOD::System* GetFmodSystem() { return m_pFmod_System; }


private:
	FMOD::System* m_pFmod_System = { nullptr };

public:
	static CFmod_System* Create();
	virtual void Free() override;
};


END

