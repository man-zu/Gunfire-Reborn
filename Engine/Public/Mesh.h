#pragma once

#include "VIBuffer.h"
#include "Model.h"

#include <json/nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json;

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	using VERTICESDESC = struct VerticeDesc
	{
		_float3 vPosition	{ 0.f, 0.f, 0.f };
		_float3 vNormal		{ 0.f, 0.f, 0.f };
		_float2 vTexCoords	{ 0.f, 0.f};
		_float3 vTangent	{ 0.f, 0.f, 0.f };
	};

public:
	using MESHDESC = struct MeshDesc
	{
		string				szName;
		_uint				iMaterialIndex;
		_uint				iNumVertices;
		_uint				mNumFaces;
		vector<_float3*>	mIndicesVec = {};
		vector<_float3*>	mPositionVec = {};
		vector<_float3*>	mNormalVec = {};
		vector<_float2*>	mTexCoordVec = {};
		vector<_float3*>	mTangentVec = {};
		_uint mNumMaterials = { 0 };
	};

private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, vector<class CBone*>& Bones, _fmatrix TransformMatrix);
	virtual HRESULT NoAi_Initialize_Prototype(const MESHDESC tDesc, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

	void SetUp_BoneMatrices(_float4x4* pMatrices, const vector<class CBone*>& Bones);


private:

	_char				m_szName[MAX_PATH] = {};
	_uint				m_iMaterialIndex = {};
	_uint				m_iNumBones = {};

	/* 모델 전체의 뼈들 중, 이 메시에 영향을 주는 뼈의 인덱스를 저장했다. */
	vector<_uint>		m_Bones;
	vector<_float4x4>	m_OffsetMatrices;

	_float3*			m_pPickingVertices = nullptr;
	//FACEINDICES32*		m_pPickingIndices = nullptr;

private:
	HRESULT Ready_VIBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix TransformMatrix);
	HRESULT NoAiReady_VIBuffer_For_NonAnim(const MESHDESC tDesc, _fmatrix TransformMatrix);
	HRESULT Ready_VIBuffer_For_Anim(const aiMesh* pAIMesh, vector<class CBone*>& Bones);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, vector<class CBone*>& Bones, _fmatrix TransformMatrix);
	static CMesh* NoAiCreate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MESHDESC tDesc, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END