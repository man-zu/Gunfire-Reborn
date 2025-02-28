#pragma once

#include "Component.h"
#include "Animation.h"

/* 모델러분들이 작성해놓은 정점의 정보와 인덱스의 정보를 로드하여 내가 사용하기좋도록 정리한다. */
/* Vertex -> Polygon -> Mesh -> Model */

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_bool Get_Finished() {
		return m_Animations[m_iCurrentAnimIndex]->Get_Finished();
	}

	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;

	void Set_AnimIndex(_uint iAnimIndex, _bool isLoop) {
		m_Animations[m_iCurrentAnimIndex]->Reset();
		m_iCurrentAnimIndex = iAnimIndex;
		m_isLoop = isLoop;
	}

	void PlayerLoadGun(_uint iAnimIndex, _bool isLoop) {
		m_Animations[m_iCurrentAnimIndex]->Reset();
		m_iCurrentAnimIndex = iAnimIndex;
		m_isLoop = isLoop;
	}

	void Set_AnimStop() {
		m_Animations[m_iCurrentAnimIndex]->Reset();
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_float fTimeDelta);
	HRESULT Bind_Buffers(_uint iMeshIndex);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	_uint getCurAnimIdx() { return m_iCurrentAnimIndex; }

private:
	/* 디자이너가 제공해준 파일의 정보를 읽어서 aiScene에 저장해주는 역활을 수행한다. */
	Assimp::Importer			m_Importer = {};

	/* 모델에 대한 모든 정보를 다 들고 있는 객체다. */
	const aiScene* m_pAIScene = { nullptr };

private:
	TYPE							m_eModelType = { TYPE_END };
	_float4x4						m_TransformMatrix = {};

private:
	_uint							m_iNumMeshes = {};
	vector<class CMesh*>			m_Meshes;

private:
	_uint							m_iNumMaterials = {};
	vector<MATERIAL_MESH>			m_Materials;

private:
	/* 이 모델 전체의 뼈들을 모아놓은 벡터 컨테이너. */
	vector<class CBone*>			m_Bones;

private:
	_bool							m_isLoop = { false };
	_uint							m_iCurrentAnimIndex = {};
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;


private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pNode, _int iParentIndex);
	HRESULT Ready_Animations();
	HRESULT SaveToJsonFile(const _char* pModelFilePath);


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END