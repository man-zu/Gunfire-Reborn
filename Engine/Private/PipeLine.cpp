#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

void CPipeLine::Set_Transform(TRANSFORMATIONSTATE eState, _fmatrix TransformationMatrix)
{
	XMStoreFloat4x4(&m_TransformationMatrices[eState], TransformationMatrix);
}

_matrix CPipeLine::Get_Transform_Matrix(TRANSFORMATIONSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformationMatrices[eState]);
}

const _float4x4 * CPipeLine::Get_Transform_Float4x4(TRANSFORMATIONSTATE eState) const
{
	return &m_TransformationMatrices[eState];
}

_matrix CPipeLine::Get_Transform_Matrix_Inverse(TRANSFORMATIONSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformationMatrix_Inverse[eState]);
}

const _float4x4 * CPipeLine::Get_Transform_Float4x4_Inverse(TRANSFORMATIONSTATE eState) const
{
	return &m_TransformationMatrix_Inverse[eState];
}

_vector CPipeLine::Get_CamPosition_Vector() const
{
	return XMLoadFloat4(&m_vCamPosition);
}

const _float4* CPipeLine::Get_CamPosition_Float4() const
{
	return &m_vCamPosition;
}

HRESULT CPipeLine::Initialize()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformationMatrices[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformationMatrix_Inverse[i], XMMatrixIdentity());
	}

	XMStoreFloat4(&m_vCamPosition, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CPipeLine::Update()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformationMatrix_Inverse[i], 
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrices[i])));
	}

	memcpy(&m_vCamPosition, &m_TransformationMatrix_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));
}

CPipeLine * CPipeLine::Create()
{
	CPipeLine*		pInstance = new CPipeLine();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPipeLine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
	__super::Free();
}
