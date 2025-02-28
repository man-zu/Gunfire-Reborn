#include "Animation.h"

#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_fDuration { rhs.m_fDuration }
	, m_fTickPerSecond { rhs.m_fTickPerSecond } 
	, m_fCurrentTrackPosition { rhs.m_fCurrentTrackPosition }
	, m_iNumChannels { rhs.m_iNumChannels }
	, m_Channels { rhs.m_Channels } 
	, m_isFinished { rhs.m_isFinished }
	, m_CurrentKeyFrameIndices { rhs.m_CurrentKeyFrameIndices }
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);	
}

HRESULT CAnimation::Initialize(const aiAnimation * pAIAnimation, const vector<CBone*>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;

	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel*			pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformationMatrix(_float fTimeDelta, const vector<CBone*>& Bones, _bool isLoop)
{
	/* 애니메이션의 재생 위치를 증가시켜준다. */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_isFinished = true;

		if (true == isLoop)
		{
			m_isFinished = false;
			m_fCurrentTrackPosition = 0.f;
		}		

		if (true == m_isFinished)
			return;
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[i]);
	}
	
}

void CAnimation::Reset()
{
	m_isFinished = false;
	m_fCurrentTrackPosition = 0;

	for (size_t i = 0; i < m_iNumChannels; i++)	
		m_CurrentKeyFrameIndices[i] = 0;	
}

CAnimation * CAnimation::Create(const aiAnimation * pAIAnimation, const vector<CBone*>& Bones)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
