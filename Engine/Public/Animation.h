#pragma once

#include "Base.h"

/* �ִϸ��̼��� �ϳ�(���ݸ��)�� ������ ������. */
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
	_float				m_fDuration = {}; /* �ִϸ��̼��� �� ��� �Ÿ�. */
	_float				m_fTickPerSecond = {}; /* �ʴ� ��� �Ÿ�.(��� �ӵ�) */
	_float				m_fCurrentTrackPosition = {}; /* ���� ��� ��ġ */

	/* �� �ִϸ��̼��� ����ϴµ� �ʿ��� ���� ����. == */
	/* �� �ִԿ��Ӽ��� ����ϴ� ���� ����. */
	_uint						m_iNumChannels = {};
	vector<class CChannel*>		m_Channels;
	/* ���� ä��(��)�� ���� ��� Ű�����ӱ��� ����Ȱ����� �����ϰ� �ִ� �����͸� */
	/* �ִϸ��̼���(�ٱ�)���� ���ػ�Ȳ. */
	vector<_uint>				m_CurrentKeyFrameIndices;

	_bool						m_isFinished = { false };


public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END