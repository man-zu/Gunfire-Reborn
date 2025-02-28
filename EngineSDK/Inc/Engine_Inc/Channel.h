#pragma once

#include "Base.h"

BEGIN(Engine)

/* 1. ���� ������ ������. */
/* 2. ���� �ִϸ��̼��� ��� �ð��� ���� ���� �������� ������. */
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
	/* KEYFRAME : �� ��(Channel)�� �ð��� ���� ����(ũ, ��, ��)�� ǥ���� ����ü.*/
	vector<KEYFRAME>		m_KeyFrames;

	_uint					m_iBoneIndex = { 0 };

	/* ���� �����ϰ� �ִ� �� Ű�������� ���� Ű������ */
	// _uint					m_iCurrentKeyFrameIndex = { 0 };


public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END