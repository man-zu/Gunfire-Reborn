#pragma once

#include "Base.h"

BEGIN(Engine)

/* 1. 뼈의 정보를 가져요. */
/* 2. 뼈의 애니메이션의 재생 시간에 따른 상태 정보들을 가진다. */
class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void Update(_float fCurrentTrackPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex);


private:
	_char					m_szName[MAX_PATH] = {};

	_uint					m_iNumKeyFrames = {};
	/* KEYFRAME : 이 뼈(Channel)의 시간에 따른 상태(크, 자, 이)를 표현한 구조체.*/
	vector<KEYFRAME>		m_KeyFrames;

	_uint					m_iBoneIndex = { 0 };

	/* 현재 보간하고 있는 두 키프레임중 왼쪽 키프레임 */
	// _uint					m_iCurrentKeyFrameIndex = { 0 };


public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END