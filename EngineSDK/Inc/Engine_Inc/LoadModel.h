#pragma once

#include "Component.h"
#include "Animation.h"
#include "Model.h"

#include <fstream>
//#include <json/nlohmann/json.hpp>
//using json = nlohmann::json;

BEGIN(Engine)

class ENGINE_DLL CLoadModel final : public CComponent
{
private:
	CLoadModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadModel(const CLoadModel& rhs);
	virtual ~CLoadModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	//_bool Get_Finished() {
	//	return m_Animations[m_iCurrentAnimIndex]->Get_Finished();
	//}

	//void Set_AnimIndex(_uint iAnimIndex, _bool isLoop) {
	//	m_Animations[m_iCurrentAnimIndex]->Reset();
	//	m_iCurrentAnimIndex = iAnimIndex;
	//	m_isLoop = isLoop;
	//
	//}

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	//void Play_Animation(_float fTimeDelta);
	//HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

private:
	/* 디자이너가 제공해준 파일의 정보를 읽어서 aiScene에 저장해주는 역할수행 */
	Assimp::Importer			m_Importer = {};
	/* 모델에 대한 모든 정보를 다 들고 있는 객체다. */
	const aiScene* m_pAIScene = { nullptr };

	//json	m_JsonAIScene = {};

private:
	CModel::TYPE					m_eModelType = { CModel::TYPE_END };
	_float4x4						m_TransformMatrix = {};

private:
	_uint							m_iNumMeshes = {};
	vector<class CMesh*>			m_Meshes;

private:
	_uint							m_iNumMaterials = {};
	vector<MATERIAL_MESH>			m_Materials;

private:
	//vector<class CBone*>			m_Bones;

private:
	//_bool							m_isLoop = { false };
	//_uint							m_iCurrentAnimIndex = {};
	//_uint							m_iNumAnimations = { 0 };
	//vector<class CAnimation*>		m_Animations;
	_uint				mNumMeshes = {0};
	_uint				mNumFaces = {0};
	_uint				mNumVertices = {0};
	_uint				mMaterialIndex = {0};
	_uint				mNumMaterials = { 0 };
	_char				m_szName[1024] = {};
	vector<_float3*>	mIndiceVec = { };
	vector<_float3*>	mPositionVec = { };
	vector<_float3*>	mNormalVec = { };
	vector<_float2*>	mTexCoordVec = { };
	vector<_float3*>	mTangentVec = { };
	vector<_char*>*		szPerfectPaths = { nullptr };

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	//HRESULT Ready_Bones(const aiNode* pNode, _int iParentIndex);
	//HRESULT Ready_Animations();
	HRESULT SaveBinaryFile(const _char* pModelFilePath, const string pModelFileOutPath);
	HRESULT LoadBinaryFile(const _char* pModelFilePath);
	//HRESULT SaveToJsonFile(const _char* pModelFilePath);
	//HRESULT LoadJsonFile(const _char* pModelFilePath);

	HRESULT FindFile(CModel::TYPE eType, const string pModelFilePath, const string pModelFileOutPath);

public:
	static HRESULT MakeData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const string pModelFilePath, const string pModelFileOutPath);
	static CLoadModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END