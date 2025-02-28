#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim * pAIChannel, const vector<CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
	{
		if (false == strcmp(m_szName, pBone->Get_Name()))
			return true;

		++m_iBoneIndex;

		return false;
	});

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3			vScale{};
	_float4			vRotation{};
	_float3			vTranslation{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = (_float)pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;		

		m_KeyFrames.emplace_back(KeyFrame);		
	}

	return S_OK;
}

void CChannel::Update(_float fCurrentTrackPosition, const vector<CBone*>& Bones, _uint* pCurrentKeyFrameIndex)
{
	if (0.0f == fCurrentTrackPosition)
		(*pCurrentKeyFrameIndex) = 0;

	/* Fin. ���� �����ġ�� �´� ���� ����� �����. */
	_matrix			TransformationMatrix = XMMatrixIdentity();

	_vector			vScale, vRotation, vTranslation;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	/* ���� ��� ��ġ�� �� ���� ������ Ű�������� �Ѿ��. */
	if (fCurrentTrackPosition > LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}
	/* ���� �����ġ�� �ΰ��� Ű�����ӵ� ���̿� �ִ�. */
	/* �ΰ��� Ű�����ӻ��̸� �����ġ�� �°� ���������Ѵ�. */
	/* (*pCurrentKeyFrameIndex) : ���� ������� Ʈ�������� ���� ���ʿ� �����ϴ� Ű������ �ΉK���� */
	else 
	{
		while (fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) / 
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation), XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), fRatio), 1.f);								
	}

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel * CChannel::Create(const aiNodeAnim * pAIChannel, const vector<CBone*>& Bones)
{
	CChannel*		pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX(TEXT("Failed to Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();
}

