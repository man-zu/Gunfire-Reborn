#pragma once

#include "Base.h"

/* assimp���� ���� ǥ���ϴ� Ÿ�� : 3���� .*/
/* aiNode, aiBone, aiNodeAnim */

/* aiNode : ���� ��Ӱ��踦 ǥ���� �Ӹ� �ƴ϶� ���� ��������� ǥ���Ѵ�. */

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}

	_matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationFloat4x4Ptr() {
		return &m_CombinedTransformationMatrix;
	}

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformationMatrix);

private:
	_char				m_szName[MAX_PATH] = {};
	_float4x4			m_TransformationMatrix; /* �� ������ ���¸� �θ� �������� ǥ���� ���. */
	_float4x4			m_CombinedTransformationMatrix; /* �� ����� * �θ� ����� */
	_int				m_iParentBoneIndex = { -1 };

public:
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);
	CBone* Clone();
	virtual void Free() override;
};

END