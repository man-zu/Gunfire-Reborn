#include "LoadModel.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"

#include "iostream"
#include "io.h"

CLoadModel::CLoadModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CLoadModel::CLoadModel(const CLoadModel& rhs)
	: CComponent( rhs )
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials{ rhs.m_Materials }
	, m_TransformMatrix{ rhs.m_TransformMatrix }
{
	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& MeshMaterial : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(MeshMaterial.pTextures[i]);
	}
}

HRESULT CLoadModel::Initialize_Prototype(CModel::TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	// NONANIM 만 로드하도록 되어있음
	m_eModelType = CModel::TYPE_NONANIM;
	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
	
	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast | aiProcess_PreTransformVertices;

	//LoadJsonFile(pModelFilePath);
	LoadBinaryFile(pModelFilePath);

	cout << "Ready_Meshes Start ----------------" << endl;
	if (FAILED(Ready_Meshes()))
		return E_FAIL;
	cout << "Ready_Meshes End ----------------" << endl;
	
	
	cout << "Ready_Materials Start ----------------" << endl;
	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;
	cout << "Ready_Materials End ----------------" << endl;


	return S_OK;
}

HRESULT CLoadModel::Initialize(void* pArg)
{


	return S_OK;
}

HRESULT CLoadModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

//void CLoadModel::Play_Animation(_float fTimeDelta)
//{
//	/* 1. 이 애니메이션이 사용하는 뼈들을 현재 시간에 맞는 애니메이션의 상태대로 뼈의 상태(TransformationMatrix)를 갱신해준다. */
//	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop);
//
//	/* 2. 모든 뼈를 다시 순회하면서 CombinedTransformationMatrix를 갱신해주낟. */
//	/*CombinedTransformationMatrix = 나의 TransformationMatrix * 부모의 CombinedTransformationMatrix */
//	for (auto& pBone : m_Bones)
//	{
//		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
//	}
//
//}

HRESULT CLoadModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pTextures[eMaterialType]->Bind_ShaderResource(pShader, pConstantName);
}

//HRESULT CLoadModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
//{
//	/* 쉐이더에 던져야 할 행렬 배열정보. */
//	_float4x4	BoneMatrices[512];
//
//	/* 행렬배열은 메시에게 영향을 주는 뼈들의 행렬만 가져야하므로 메시에게 저장좀해줘! */
//	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones);
//
//	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 512);
//}

HRESULT CLoadModel::Ready_Meshes()
{
	///* 메시의 갯수를 저장하낟. */
	//m_iNumMeshes = m_JsonAIScene["mNumMeshes"];
	
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh::MESHDESC tDesc = {};
		tDesc.szName = m_szName;
		tDesc.iMaterialIndex = mMaterialIndex;
		tDesc.iNumVertices = mNumVertices;
		tDesc.mNumFaces = mNumFaces;
		tDesc.mIndicesVec = mIndiceVec;
		tDesc.mPositionVec = mPositionVec;
		tDesc.mNormalVec = mNormalVec;
		tDesc.mTexCoordVec = mTexCoordVec;
		tDesc.mTangentVec = mTangentVec;
		tDesc.mNumMaterials = mNumMaterials;

		CMesh* pMesh = CMesh::NoAiCreate(m_pDevice, m_pContext, tDesc, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CLoadModel::Ready_Materials(const _char* pModelFilePath)
{
	MATERIAL_MESH			MeshMaterial = {};

	for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
	{
		vector<_char*> path = *szPerfectPaths;
		string tmp(path[j - 1]);
		_wstring filePath;
		filePath.assign(tmp.begin(), tmp.end());

		if (tmp == "")
			continue;

		MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, filePath, 1);
		if (nullptr == MeshMaterial.pTextures[j])
			return E_FAIL;
	}

	m_Materials.emplace_back(MeshMaterial);

	return S_OK;


	//m_iNumMaterials = m_JsonAIScene["m_iNumMaterials"];
	//
	//for (size_t i = 0; i < m_iNumMaterials; i++)
	//{
	//	aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];
	//
	//	MATERIAL_MESH			MeshMaterial = {};
	//
	//	vector<string> paths;
	//	for (auto path : m_JsonAIScene["szPerfectPath"])
	//	{
	//		paths.push_back(path);
	//	}
	//
	//	for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
	//	{
	//		string tmp = paths[j - 1].c_str();
	//		_wstring filePath;
	//		filePath.assign(tmp.begin(), tmp.end());
	//
	//		if (tmp == "")
	//			continue;
	//
	//		MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, filePath, 1);
	//		if (nullptr == MeshMaterial.pTextures[j])
	//			return E_FAIL;
	//	}
	//
	//	m_Materials.emplace_back(MeshMaterial);
	//}
	//return S_OK;
}

//HRESULT CLoadModel::Ready_Bones(const aiNode* pNode, _int iParentIndex)
//{
//	CBone* pBone = CBone::Create(pNode, iParentIndex);
//	if (nullptr == pBone)
//		return E_FAIL;
//
//	m_Bones.emplace_back(pBone);
//
//	_uint		iNumChildren = pNode->mNumChildren;
//
//	_int		iNumBones = (_int)m_Bones.size();
//
//	for (size_t i = 0; i < iNumChildren; i++)
//	{
//		Ready_Bones(pNode->mChildren[i], iNumBones - 1);
//	}
//
//	return S_OK;
//}

HRESULT CLoadModel::SaveBinaryFile(const _char* pModelFilePath, const string pModelFileOutPath)
{
	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast | aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;
	aiMesh* pAIMesh = m_pAIScene->mMeshes[0];

	/* 저장할 파일명 */
	_char		szFileName[MAX_PATH] = {};
	_char		szExt[MAX_PATH] = {};
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
	string str(szFileName);
	string makeFileName = pModelFileOutPath + str + ".data";
	wstring filePath = L"";
	filePath.assign(makeFileName.begin(), makeFileName.end());



	_char m_szName[1024];
	strcpy_s(m_szName, pAIMesh->mName.data);

	std::ofstream file(makeFileName, std::ios::binary);

	file.write(reinterpret_cast<const char*>(&m_pAIScene->mNumMeshes), sizeof(_uint));
	file.write(reinterpret_cast<const char*>(&pAIMesh->mNumFaces), sizeof(_uint));
	file.write(reinterpret_cast<const char*>(&pAIMesh->mNumVertices), sizeof(_uint));
	file.write(reinterpret_cast<const char*>(&pAIMesh->mMaterialIndex), sizeof(_uint));
	file.write(reinterpret_cast<const char*>(&m_szName), sizeof(_char)* 1024);

	for (_uint j = 0; j < pAIMesh->mNumFaces; ++j)
	{
		_float3		position = { 0.f, 0.f, 0.f };
		position.x = pAIMesh->mFaces[j].mIndices[0];
		position.y = pAIMesh->mFaces[j].mIndices[1];
		position.z = pAIMesh->mFaces[j].mIndices[2];
		file.write(reinterpret_cast<const char*>(&position), sizeof(_float3));
	}

	/* meshes */
	for (_uint i = 0; i < pAIMesh->mNumVertices; ++i)
	{
		_float3		position = {0.f, 0.f, 0.f};
		position.x = pAIMesh->mVertices[i].x;
		position.y = pAIMesh->mVertices[i].y;
		position.z = pAIMesh->mVertices[i].z;
		file.write(reinterpret_cast<const char*>(&position), sizeof(_float3));

		_float3		normal = { 0.f, 0.f, 0.f };
		normal.x = pAIMesh->mNormals[i].x;
		normal.y = pAIMesh->mNormals[i].y;
		normal.z = pAIMesh->mNormals[i].z;
		file.write(reinterpret_cast<const char*>(&normal), sizeof(_float3));

		_float2		texCoords = { 0.f, 0.f };
		texCoords.x = pAIMesh->mTextureCoords[0][i].x;
		texCoords.y = pAIMesh->mTextureCoords[0][i].y;
		file.write(reinterpret_cast<const char*>(&texCoords), sizeof(_float2));

		_float3		mTangents = { 0.f, 0.f, 0.f };
		mTangents.x = pAIMesh->mTangents[i].x;
		mTangents.y = pAIMesh->mTangents[i].y;
		mTangents.z = pAIMesh->mTangents[i].z;
		file.write(reinterpret_cast<const char*>(&mTangents), sizeof(_float3));
	}

	cout << str << ")) m_pAIScene->mNumMaterials : " << m_pAIScene->mNumMaterials << endl;
	file.write(reinterpret_cast<const char*>(&m_pAIScene->mNumMaterials), sizeof(_uint));

	for (_uint i = 0; i < m_pAIScene->mNumMaterials; ++i)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString		strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
			{
				_char szPerfectPath[MAX_PATH] = "";
				file.write(reinterpret_cast<const char*>(&szPerfectPath), sizeof(_char) * MAX_PATH);
				continue;
			}
			_char		szFullPath[MAX_PATH] = {};
			/* 모델파일경로로부터 뜯어온 문자열. */
			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};

			/* 모델파일(aiScene)로부터 뜯어온 문자열. */
			_char		szImgFileName[MAX_PATH] = {};
			_char		szImgExt[MAX_PATH] = {};

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szImgFileName, MAX_PATH, szImgExt, MAX_PATH);

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szImgFileName);
			strcat_s(szFullPath, szImgExt);

			_char szPerfectPath[MAX_PATH];
			strcpy_s(szPerfectPath, szFullPath);

			file.write(reinterpret_cast<const char*>(&szPerfectPath), sizeof(_char)* MAX_PATH);
		}
	}

	file.close();

	return S_OK;
}

HRESULT CLoadModel::LoadBinaryFile(const _char* pModelFilePath)
{
	string str(pModelFilePath);
	wstring filePath = L"";
	filePath.assign(str.begin(), str.end());


	vector<_float3*> IndiceVec{};
	vector<_float3*> PositionVec{};
	vector<_float3*> NormalVec{};
	vector<_float2*> TexCoordVec{};
	vector<_float3*> TangentVec{};
	vector<_char*>* PerfectPathVec = new vector<_char*>;


	/* 바이너리 읽기 */
	std::ifstream file(pModelFilePath, std::ios::binary | std::ios::out);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	file.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	file.read(reinterpret_cast<char*>(&mNumFaces), sizeof(_uint));
	file.read(reinterpret_cast<char*>(&mNumVertices), sizeof(_uint));
	file.read(reinterpret_cast<char*>(&mMaterialIndex), sizeof(_uint));
	file.read(reinterpret_cast<char*>(&m_szName), sizeof(_char)* 1024);

	for (_uint j = 0; j < mNumFaces; ++j)
	{
		_float3* pIndice = new _float3;
		file.read(reinterpret_cast<char*>(pIndice), sizeof(_float3));
		IndiceVec.push_back(pIndice);
	}

	/* meshes */
	for (_uint i = 0; i < mNumVertices; ++i)
	{
		_float3*	position = new _float3;
		file.read(reinterpret_cast<char*>(position), sizeof(_float3));
		PositionVec.push_back(position);
	
		_float3*	normal = new _float3;
		file.read(reinterpret_cast<char*>(normal), sizeof(_float3));
		NormalVec.push_back(normal);
	
		_float2*	texCoords = new _float2;
		file.read(reinterpret_cast<char*>(texCoords), sizeof(_float2));
		TexCoordVec.push_back(texCoords);
	
		_float3*	mTangent = new _float3;
		file.read(reinterpret_cast<char*>(mTangent), sizeof(_float3));
		TangentVec.push_back(mTangent);
	}
	
	file.read(reinterpret_cast<char*>(&mNumMaterials), sizeof(_uint));
	
	for (_uint i = 0; i < mNumMaterials; ++i)
	{
		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			_char* szPerfectPath = new _char[MAX_PATH];
			ZeroMemory(szPerfectPath, sizeof(_char*));
			file.read(reinterpret_cast<char*>(szPerfectPath), sizeof(_char) * MAX_PATH);
			PerfectPathVec->push_back(szPerfectPath);
		}
	}

	file.close();

	for (auto& Indc : IndiceVec)
	{
		mIndiceVec.push_back(Indc);
	}

	for (auto& pos : PositionVec)
	{
		mPositionVec.push_back(pos);
	}
	
	for (auto& nor : NormalVec)
	{
		mNormalVec.push_back(nor);
	}
	
	for (auto& texCoord : TexCoordVec)
	{
		mTexCoordVec.push_back(texCoord);
	}
	
	for (auto& tan : TangentVec)
	{
		mTangentVec.push_back(tan);
	}

	memcpy(&szPerfectPaths, &PerfectPathVec, sizeof(vector<_char*>*));

	return S_OK;

}

//HRESULT CLoadModel::Ready_Animations()
//{
//	m_iNumAnimations = m_pAIScene->mNumAnimations;
//
//	for (size_t i = 0; i < m_iNumAnimations; i++)
//	{
//		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
//		if (nullptr == pAnimation)
//			return E_FAIL;
//
//		m_Animations.emplace_back(pAnimation);
//	}
//
//	return S_OK;
//}
//

/*
//HRESULT CLoadModel::SaveToJsonFile(const _char* pModelFilePath)
//{
//	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast | aiProcess_PreTransformVertices;
//
//	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
//	if (nullptr == m_pAIScene)
//		return E_FAIL;
//
//	_char		szFileName[MAX_PATH] = {};
//	_char		szExt[MAX_PATH] = {};
//	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
//
//
//	aiMesh* pAIMesh = m_pAIScene->mMeshes[0];
//	json jsonVertex;
//
//	jsonVertex["mNumMeshes"] = m_pAIScene->mNumMeshes;
//	jsonVertex["mNumFaces"] = pAIMesh->mNumFaces;
//	jsonVertex["mNumVertices"] = pAIMesh->mNumVertices;
//	jsonVertex["m_szName"] = pAIMesh->mName.data;
//	jsonVertex["m_iMaterialIndex"] = pAIMesh->mMaterialIndex;
//
//	for (_uint j = 0; j < pAIMesh->mNumFaces; ++j)
//	{
//		json arrValue;
//
//		arrValue.push_back(pAIMesh->mFaces[j].mIndices[0]);
//		arrValue.push_back(pAIMesh->mFaces[j].mIndices[1]);
//		arrValue.push_back(pAIMesh->mFaces[j].mIndices[2]);
//		jsonVertex["mIndices"].emplace_back(arrValue);
//		arrValue.clear();
//	}
//
//	json meshes;
//	for (_uint i = 0; i < pAIMesh->mNumVertices; ++i)
//	{
//		json arrValue;
//		
//		arrValue.push_back(pAIMesh->mVertices[i].x);
//		arrValue.push_back(pAIMesh->mVertices[i].y);
//		arrValue.push_back(pAIMesh->mVertices[i].z);
//		meshes["position"] = arrValue;
//		arrValue.clear();
//
//		if (pAIMesh->HasNormals())
//		{
//			arrValue.push_back(pAIMesh->mNormals[i].x);
//			arrValue.push_back(pAIMesh->mNormals[i].y);
//			arrValue.push_back(pAIMesh->mNormals[i].z);
//			meshes["normal"] = arrValue;
//			arrValue.clear();
//		}
//
//		if (pAIMesh->HasTextureCoords(0))
//		{
//			arrValue.push_back(pAIMesh->mTextureCoords[0][i].x);
//			arrValue.push_back(pAIMesh->mTextureCoords[0][i].y);
//			meshes["texCoords"] = arrValue;
//			arrValue.clear();
//		}
//
//		if (pAIMesh->HasTangentsAndBitangents())
//		{
//			arrValue.push_back(pAIMesh->mTangents[i].x);
//			arrValue.push_back(pAIMesh->mTangents[i].y);
//			arrValue.push_back(pAIMesh->mTangents[i].z);
//			meshes["tangent"] = arrValue;
//			arrValue.clear();
//		}
//		jsonVertex["meshes"].emplace_back(meshes);
//		meshes.clear();
//	}
//
//	for (_uint i = 0; i < m_pAIScene->mNumMaterials; ++i)
//	{
//		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];
//		json aMaterialArr;
//
//		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
//		{
//			aiString		strTexturePath;
//
//			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
//			{
//				string str("");
//				aMaterialArr.emplace_back(str);
//				continue;
//			}
//			_char		szFullPath[MAX_PATH] = {};
//			// 모델파일경로로부터 뜯어온 문자열.
//			_char		szDrive[MAX_PATH] = {};
//			_char		szDir[MAX_PATH] = {};
//
//			// 모델파일(aiScene)로부터 뜯어온 문자열.
//			_char		szImgFileName[MAX_PATH] = {};
//			_char		szImgExt[MAX_PATH] = {};
//
//			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
//			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szImgFileName, MAX_PATH, szImgExt, MAX_PATH);
//
//			strcpy_s(szFullPath, szDrive);
//			strcat_s(szFullPath, szDir);
//			strcat_s(szFullPath, szImgFileName);
//			strcat_s(szFullPath, szImgExt);
//
//			_tchar		szPerfectPath[MAX_PATH] = {};
//
//			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szPerfectPath, MAX_PATH);
//
//			wstring ws(szPerfectPath);
//			string str(szFullPath);
//
//			aMaterialArr.emplace_back(str);
//		}
//		jsonVertex["szPerfectPath"] = aMaterialArr;
//	}
//
//	// JSON -> 바이너리 로 저장하기
//	string str(szFileName);
//	string makeFileName = "../Bin/ToolSavedFiles/bin/" + str + ".data";
//
//	std::vector<uint8_t> binary_data;
//	std::string json_str = jsonVertex.dump();
//	binary_data.assign(json_str.begin(), json_str.end());
//	
//	std::ofstream file(makeFileName, std::ios::binary);
//	file.write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
//
//	file.close();
//
//	// /* json 으로 파일저장 */
//	//std::ofstream ofs2(makeFile);
//	//if (!ofs2.is_open()) {
//	//	std::cout << "Failed to open file for writing!" << std::endl;
//	//	//return E_FAIL;
//	//}
//	//
//	//ofs2 << std::setw(4) << jsonVertex << std::endl;
//	//ofs2.close();
//
//	std::cout << "filepath : " + makeFileName << std::endl;
//
//	return S_OK;
//}
//
//HRESULT CLoadModel::LoadJsonFile(const _char* pModelFilePath)
//{
//	cout << "pModelFilePath 파일 로드중 : " << pModelFilePath << endl;
//	try {
//
//		// /* 바이너리 읽기 */
//		std::ifstream file(pModelFilePath, std::ios::binary | std::ios::ate);
//		std::streamsize size = file.tellg();
//		file.seekg(0, std::ios::beg);
//		
//		std::vector<uint8_t> binary_data(size);
//		if (!file.read(reinterpret_cast<char*>(binary_data.data()), size)) {
//			return E_FAIL;
//		}
//
//		std::string json_str(binary_data.begin(), binary_data.end());
//		json jDataList = json::parse(json_str);
//		file.close();
//		cout << "=========================== 파일 로드끝 : " << endl;
//
//		// /* json 읽기 */
//		//std::ifstream file(pModelFilePath, std::ios::in);
//		//if (!file.is_open())
//		//{
//		//	return E_FAIL;
//		//}
//		//
//		//json jDataList;
//		//file >> jDataList;
//		//
//		//file.close();
//
//		m_JsonAIScene["meshes"] = jDataList["meshes"];
//		m_JsonAIScene["mIndices"] = jDataList["mIndices"];
//		m_JsonAIScene["mNumFaces"] = jDataList["mNumFaces"];
//		m_JsonAIScene["mNumVertices"] = jDataList["mNumVertices"];
//		m_JsonAIScene["mNumMeshes"] = jDataList["mNumMeshes"];
//		m_JsonAIScene["m_szName"] = jDataList["m_szName"];
//		m_JsonAIScene["szPerfectPath"] = jDataList["szPerfectPath"];
//		m_JsonAIScene["m_iMaterialIndex"] = jDataList["m_iMaterialIndex"];
//
//		return S_OK;
//	}
//	catch (std::exception& e) {
//		return E_FAIL;
//	}
//}
//*/

HRESULT CLoadModel::FindFile(CModel::TYPE eType, const string pModelFilePath, const string pModelFileOutPath)
{
	//string path = "../Bin/Resources/Models/Object/*.*";
	//string path2 = "../Bin/Resources/Models/Object/";
	string path = pModelFilePath + "*.*";
	string path2 = pModelFilePath+ "";

	struct _finddata_t fd;
	intptr_t handle;
	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
		cout << "No file in directory!" << endl;

	do
	{
		cout << fd.name << endl;
		string filePath = path2 + fd.name;

		if (strchr(fd.name, 'FBX') || strchr(fd.name, 'fbx'))
		{
			if (FAILED(SaveBinaryFile(filePath.c_str(), pModelFileOutPath)))
			{
				MSG_BOX(TEXT("Failed to Created : CLoadModel"));
				return E_FAIL;
			}
			

			//if (FAILED(SaveToJsonFile(filePath.c_str())))
			//{
			//	MSG_BOX(TEXT("Failed to Created : CLoadModel"));
			//	return E_FAIL;
			//}
		}


	} while (_findnext(handle, &fd) == 0);
	_findclose(handle);


	std::cout << "===================================================" << std::endl;
	std::cout << " 파일이 성공적으로 저장되었습니다." << std::endl;

	return S_OK;
}

HRESULT CLoadModel::MakeData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const string pModelFilePath, const string pModelFileOutPath)
{
	CLoadModel* pInstance = new CLoadModel(pDevice, pContext);
	
	if (FAILED(pInstance->FindFile(eType, pModelFilePath, pModelFileOutPath)))
	{
		MSG_BOX(TEXT("Failed to Created : MakeData"));
		Safe_Release(pInstance);
		return E_FAIL;
	}

	return S_OK;
}

CLoadModel* CLoadModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const _char* pModelFilePath, _fmatrix TransformMatrix)
{
	CLoadModel* pInstance = new CLoadModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoadModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CLoadModel::Clone(void* pArg)
{
	CLoadModel* pInstance = new CLoadModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLoadModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadModel::Free()
{
	__super::Free();

	//for (auto& pAnimation : m_Animations)
	//	Safe_Release(pAnimation);
	//m_Animations.clear();
	//
	//for (auto& pBone : m_Bones)
	//	Safe_Release(pBone);
	//
	//m_Bones.clear();

	for (auto& MeshMaterial : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release(MeshMaterial.pTextures[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	if(0 < mIndiceVec.size())
	{
		for (auto& tmp : mIndiceVec)
			Safe_Delete(tmp);
		mIndiceVec.clear();
	}

	if (0 < mPositionVec.size())
	{
		for (auto& tmp : mPositionVec)
			Safe_Delete(tmp);
		mPositionVec.clear();
	}

	if (0 < mNormalVec.size())
	{
		for (auto& tmp : mNormalVec)
			Safe_Delete(tmp);
		mNormalVec.clear();
	}

	if (0 < mTexCoordVec.size())
	{
		for (auto& tmp : mTexCoordVec)
			Safe_Delete(tmp);
		mTexCoordVec.clear();
	}

	if (0 < mTangentVec.size())
	{
		for (auto& tmp : mTangentVec)
			Safe_Delete(tmp);
		mTangentVec.clear();
	}


	if (szPerfectPaths)
	{
		for (auto* pPath : *szPerfectPaths)
			Safe_Delete(pPath);
		szPerfectPaths->clear();
		Safe_Delete(szPerfectPaths);
	}

}
