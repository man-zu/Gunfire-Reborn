#pragma once

#include "Base.h"

/* 애니메이션의 하나(공격모션)의 정보를 가진다. */
BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	_bool Get_Finished() const {
		return m_isFinished;
	}

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);
	void Reset();

private:
	_char				m_szName[MAX_PATH] = {};
	_float				m_fDuration = {}; /* 애니메이션의 총 재생 거리. */
	_float				m_fTickPerSecond = {}; /* 초당 재생 거리.(재생 속도) */
	_float				m_fCurrentTrackPosition = {}; /* 현재 재생 위치 */

	/* 이 애니메이션을 재생하는데 필요한 뼈의 갯수. == */
	/* 이 애님에ㅣ션이 사용하는 뼈의 갯수. */
	_uint						m_iNumChannels = {};
	vector<class CChannel*>		m_Channels;
	/* 원래 채널(뼈)이 현재 어디 키프레임까지 재생된건지를 저장하고 있던 데이터를 */
	/* 애니메이션쪽(바깥)으로 빼준상황. */
	vector<_uint>				m_CurrentKeyFrameIndices;

	_bool						m_isFinished = { false };


public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END