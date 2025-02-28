#pragma once

#include "Base.h"

/* ��, ��������� �����Ѵ�. */
/* Update�Լ����� ��, ���� �̳���� ������� ���ؼ� �����Ѵ�. */
/* �����ش�. */

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum TRANSFORMATIONSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();	
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORMATIONSTATE eState, _fmatrix TransformationMatrix);

public:
	_matrix Get_Transform_Matrix(TRANSFORMATIONSTATE eState) const;
	const _float4x4* Get_Transform_Float4x4(TRANSFORMATIONSTATE eState) const;
	_matrix Get_Transform_Matrix_Inverse(TRANSFORMATIONSTATE eState) const;
	const _float4x4* Get_Transform_Float4x4_Inverse(TRANSFORMATIONSTATE eState) const;
	_vector Get_CamPosition_Vector() const;
	const _float4* Get_CamPosition_Float4() const;
		

public:
	HRESULT Initialize();
	void Update();

private:
	_float4x4			m_TransformationMatrices[D3DTS_END];
	_float4x4			m_TransformationMatrix_Inverse[D3DTS_END];
	_float4				m_vCamPosition = {};

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END