#pragma once

#include "Base.h"

/* assimp에서 뼈를 표현하는 타입 : 3가지 .*/
/* aiNode, aiBone, aiNodeAnim */

/* aiNode : 뼈의 상속관계를 표현할 뿐만 아니라 뼈의 상태행렬을 표현한다. */

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
	_float4x4			m_TransformationMatrix; /* 내 뼈만의 상태를 부모 기준으로 표현한 행렬. */
	_float4x4			m_CombinedTransformationMatrix; /* 내 뼈행렬 * 부모 뼈행렬 */
	_int				m_iParentBoneIndex = { -1 };

public:
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);
	CBone* Clone();
	virtual void Free() override;
};

END