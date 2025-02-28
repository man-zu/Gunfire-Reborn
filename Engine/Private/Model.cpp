#include "Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"


CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials{ rhs.m_Materials }
	/*, m_Bones{ rhs.m_Bones }*/
	, m_iNumAnimations{ rhs.m_iNumAnimations }
	/*, m_Animations {rhs.m_Animations }*/
	, m_TransformMatrix{ rhs.m_TransformMatrix }
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.emplace_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& MeshMaterial : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(MeshMaterial.pTextures[i]);
	}
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (false == strcmp(pBone->Get_Name(), pBoneName))
				return true;

			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationFloat4x4Ptr();
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	/* aiProcess_PreTransformVertices : 내 모델은 여러개의 메시(칼, 몸, 가방, 모닝스타, 그림자)로 구분되어있어. */
	/* 이 각각의 메시를 배치되야할 상태대로 다 미리 셋팅하여 로드한다. */
	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (eType == TYPE_NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	/* 모든 모델의 정보를 다 읽어서 aiScene안에 저장해둔 것이다. */
	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_eModelType = eType;

	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	/* aiScene에 있는 정보를 내가 활용하기 좋도록 수정 정리 보관하는 작업을 수행하자. */
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	/* 이 모델을 구성하는 각각의 메시를 로드하자. */
	/* 애님 모델일 경우, 현재 메시가 사용하는 뼈의 인덱스를 메시에 보관한다. */
	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	/* 빛 정보 <-> 정점의 노말, (Material) 픽셀당 무슨 색을 반사해야할지?! 픽셀당 환경광원을 어떻게 설정할지? 픽셀당 스펙큘러 연산을 어떻게해야할지?! */
	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	//if (eType == TYPE_NONANIM)
	//{
	//	if (FAILED(SaveToJsonFile(pModelFilePath)))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{


	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	/* old : 모든 메시를 순회하면서 각 메시의 렌더함수를 호출한다. */
	/* Curr : 메시의 순회를 클라이언트에서 수행을 하고 지정한 메시에 대해서만 렌더함수를 호출한다.  */
	/* 어떤 머테리얼(Diffuse Ambient)을 어떤 쉐이더에 어떤 변수에 전달할 건지를 결정을 해야하는 작업을 수행해야했다. */
	/* 어떤 머테리얼인지가 메시마다 다르다. CMesh::m_iMaterialIndex */

	//for (size_t i = 0; i < m_iNumMeshes; i++)
	//{
	//	/* 쉐이더에 텍스쳐를 던지느 ㄴ작업을 클라이언트에서 처리하기위해 위의 루프자체를 클라로 옮긴것이다. */
	//	m_Materials[m_Meshes[i]->Get_MaterialIndex()].pTextures[aiTextureType_DIFFUSE]->Bind_ShaderResource();

	//	m_Meshes[i]->Render();
	//}

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Play_Animation(_float fTimeDelta)
{
	/* 1. 이 애니메이션이 사용하는 뼈들을 현재 시간에 맞는 애니메이션의 상태대로 뼈의 상태(TransformationMatrix)를 갱신해준다. */
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop);

	/* 2. 모든 뼈를 다시 순회하면서 CombinedTransformationMatrix를 갱신해주낟. */
	/*CombinedTransformationMatrix = 나의 TransformationMatrix * 부모의 CombinedTransformationMatrix */
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}

}

HRESULT CModel::Bind_Buffers(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_Buffers();
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pTextures[eMaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	/* 쉐이더에 던져야 할 행렬 배열정보. */
	_float4x4	BoneMatrices[512];

	/* 행렬배열은 메시에게 영향을 주는 뼈들의 행렬만 가져야하므로 메시에게 저장좀해줘! */
	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones);

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 512);
}

HRESULT CModel::Ready_Meshes()
{
	/* 메시의 갯수를 저장하낟. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	/* 모델에는 여러개의 머테리얼이 존재하낟. */
	/* 각 머테리얼마다 다양한 재질(Diffuse, Ambient,Specular...)을 텍스쳐로 표현한다. */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MATERIAL_MESH			MeshMaterial = {};

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			/* 얻어온 경로에서 신용할 수 있는 건 파일의 이름과 확장자. */
			aiString		strTexturePath;

			/* 내가 지정한 텍스쳐의 경로를 얻어주낟.*/
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			_char		szFullPath[MAX_PATH] = {};
			/* 모델파일경로로부터 뜯어온 문자열. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* 모델파일(aiScene)로부터 뜯어온 문자열. */
			_char		szFileName[MAX_PATH] = {};
			_char		szExt[MAX_PATH] = {};
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);


			_tchar		szPerfectPath[MAX_PATH] = {};

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);

			MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath, 1);
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pNode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pNode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_uint		iNumChildren = pNode->mNumChildren;

	_int		iNumBones = (_int)m_Bones.size();

	for (size_t i = 0; i < iNumChildren; i++)
	{
		Ready_Bones(pNode->mChildren[i], iNumBones - 1);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::SaveToJsonFile(const _char* pModelFilePath)
{
	_char		szFileName[MAX_PATH] = {};
	_char		szExt[MAX_PATH] = {};
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);


	aiMesh* pAIMesh = m_pAIScene->mMeshes[0];
	json jsonVertex;

	jsonVertex["mNumMeshes"] = m_pAIScene->mNumMeshes;
	jsonVertex["mNumFaces"] = pAIMesh->mNumFaces;
	jsonVertex["mNumVertices"] = pAIMesh->mNumVertices;

	for (_uint i = 0; i < pAIMesh->mNumVertices; ++i)
	{
		json arrValue;

		for (_uint j = 0; j < pAIMesh->mNumFaces; ++j)
		{
			arrValue.push_back(pAIMesh->mFaces[j].mIndices[0]);
			arrValue.push_back(pAIMesh->mFaces[j].mIndices[1]);
			arrValue.push_back(pAIMesh->mFaces[j].mIndices[2]);
			jsonVertex["mIndices"].emplace_back(arrValue);
			arrValue.clear();

		}

		for (_uint j = 0; j < m_pAIScene->mNumMeshes; ++j)
		{
			m_pAIScene->mMeshes[i];

			arrValue.clear();
		}

		arrValue.push_back(pAIMesh->mVertices[i].x);
		arrValue.push_back(pAIMesh->mVertices[i].y);
		arrValue.push_back(pAIMesh->mVertices[i].z);
		jsonVertex["position"] = arrValue;
		arrValue.clear();

		if (pAIMesh->HasNormals())
		{
			arrValue.push_back(pAIMesh->mNormals[i].x);
			arrValue.push_back(pAIMesh->mNormals[i].y);
			arrValue.push_back(pAIMesh->mNormals[i].z);
			jsonVertex["normal"] = arrValue;
			arrValue.clear();
		}

		if (pAIMesh->HasTextureCoords(0))
		{
			arrValue.push_back(pAIMesh->mTextureCoords[0][i].x);
			arrValue.push_back(pAIMesh->mTextureCoords[0][i].y);
			jsonVertex["texCoords"] = arrValue;
			arrValue.clear();
		}

		if (pAIMesh->HasTangentsAndBitangents())
		{
			arrValue.push_back(pAIMesh->mTangents[i].x);
			arrValue.push_back(pAIMesh->mTangents[i].y);
			arrValue.push_back(pAIMesh->mTangents[i].z);
			jsonVertex["tangent"] = arrValue;
			arrValue.clear();
		}
	}


	for (_uint i = 0; i < m_pAIScene->mNumMaterials; ++i)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];
		json aMaterialArr;

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString		strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
			{
				continue;
			}
			_char		szFullPath[MAX_PATH] = {};
			/* 모델파일경로로부터 뜯어온 문자열. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* 모델파일(aiScene)로부터 뜯어온 문자열. */
			//_char		szFileName[MAX_PATH] = {};
			//_char		szExt[MAX_PATH] = {};

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			//_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szPerfectPath[MAX_PATH] = {};

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szPerfectPath, MAX_PATH);

			wstring ws(szPerfectPath);
			string str(szFullPath);

			aMaterialArr.emplace_back(str);
		}
		jsonVertex["szPerfectPath"] = aMaterialArr;
	}

	//json jDataList = json::parse(jsonVertex);

	// JSON 파일에 쓰기
	std::ofstream ofs2("../Bin/ToolSavedFiles/test/" + string(szFileName) + ".json");
	if (!ofs2.is_open()) {
		//std::cout << "Failed to open file for writing!2222" << std::endl;
		//return E_FAIL;
	}

	ofs2 << std::setw(4) << jsonVertex << std::endl;
	ofs2.close();

	//std::cout << "JSON 파일이 성공적으로 저장되었습니다." << std::endl;

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& MeshMaterial : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release(MeshMaterial.pTextures[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();
}
