#include "Mesh.h"
#include "Bone.h"

using namespace std;
CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer( rhs )
{
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, vector<class CBone*>& Bones, _fmatrix TransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;


	m_iIndexStride = 4;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT	hr = eModelType == CModel::TYPE_NONANIM ? Ready_VIBuffer_For_NonAnim(pAIMesh, TransformMatrix) : Ready_VIBuffer_For_Anim(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	/* �ﰢ���� ������ ����. */
	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::NoAi_Initialize_Prototype(const MESHDESC tDesc, _fmatrix TransformMatrix)
{
	string szName = tDesc.szName;
	strcpy_s(m_szName, szName.c_str());
	m_iMaterialIndex = tDesc.iMaterialIndex;

	m_iIndexStride = 4;
	m_iNumVertices = tDesc.iNumVertices; //pAIMesh->mNumVertices;
	m_iNumIndices = tDesc.mNumFaces * 3;//pAIMesh->mNumFaces * 3;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	if (FAILED(NoAiReady_VIBuffer_For_NonAnim(tDesc, TransformMatrix)))
		return E_FAIL;
#pragma endregion


#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	/* �ﰢ���� ������ ����. */
	for (size_t i = 0; i < tDesc.mNumFaces; i++)
	{
		pIndices[iNumIndices++] = tDesc.mIndicesVec[i]->x;
		pIndices[iNumIndices++] = tDesc.mIndicesVec[i]->y;
		pIndices[iNumIndices++] = tDesc.mIndicesVec[i]->z;
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMesh::SetUp_BoneMatrices(_float4x4* pMatrices, const vector<class CBone*>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&pMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_Bones[i]]->Get_CombinedTransformationMatrix());
	}
}

/* ���� ���� */
HRESULT CMesh::Ready_VIBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix TransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	/* �����ϰ����ϴ� ������ �Ӽ��� �����ϳ�. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* �����ϰ����ϴ� ������ �ʱⰪ�� �����Ѵ�. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), TransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), TransformMatrix));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::NoAiReady_VIBuffer_For_NonAnim(const MESHDESC tDesc, _fmatrix TransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	/* �����ϰ����ϴ� ������ �Ӽ��� �����ϳ�. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* �����ϰ����ϴ� ������ �ʱⰪ�� �����Ѵ�. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	vector<VERTICESDESC> vecVertices;

	//for (auto mesh : jMesh["meshes"])
	for (_uint j = 0; j < m_iNumVertices; ++j)
	{

		VERTICESDESC verDesc;
		verDesc.vNormal = { tDesc.mNormalVec[j]->x, tDesc.mNormalVec[j]->y, tDesc.mNormalVec[j]->z };
		verDesc.vPosition = { tDesc.mPositionVec[j]->x, tDesc.mPositionVec[j]->y, tDesc.mPositionVec[j]->z};
		verDesc.vTexCoords = { tDesc.mTexCoordVec[j]->x , tDesc.mTexCoordVec[j]->y};
		verDesc.vTangent = { tDesc.mTangentVec[j]->x, tDesc.mTangentVec[j]->y, tDesc.mTangentVec[j]->z};
		vecVertices.push_back(verDesc);
	}

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &vecVertices[i].vPosition, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), TransformMatrix));

		memcpy(&pVertices[i].vNormal, &vecVertices[i].vNormal, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), TransformMatrix));

		memcpy(&pVertices[i].vTexcoord, &vecVertices[i].vTexCoords, sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &vecVertices[i].vTangent, sizeof(_float3));
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VIBuffer_For_Anim(const aiMesh* pAIMesh, vector<class CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	/* �����ϰ����ϴ� ������ �Ӽ��� �����ϳ�. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* �����ϰ����ϴ� ������ �ʱⰪ�� �����Ѵ�. */
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	/* �� �޽ÿ� ������ �ִ� ���� ����. */
	m_iNumBones = pAIMesh->mNumBones;

	/* ������ ��ȸ�ϴ� ���� : */
	/* aiMesh->aiBone�ȿ� �� ���� ������ �ִ� �޽ñ����� ������ �ε�����
	�������� ������ �ִ� Weights���� ������ �ֱ� ������ */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		/* �� �޽ÿ� ������ �ִ� ���ŵ� �� i��° ��. */
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4		OffsetMatrix{};
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.emplace_back(OffsetMatrix);

		_uint			iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (false == strcmp(pAIBone->mName.data, pBone->Get_Name()))
					return true;

				++iBoneIndex;

				return false;
			});

		if (iter == Bones.end())
			return E_FAIL;

		m_Bones.emplace_back(iBoneIndex);

		/* �� i��° ���� ��� �������� ������ �ִµ�? */
		/* i��° ���� ������ �ִ� ���������� ��ȸ�Ѵ�. */
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			/* pAIBone->mWeights[j].mVertexId : �� ���� j��° ������ �ִ� ������ �ε��� */
			/* pAIBone->mWeights[j].mWeight : �� ���� j��° ������ �ִ� pAIBone->mWeights[j].mVertexId��° �������� �� �۳� ������ �ִ°�? */

			/* ������ �װ��� ���� ������ ���� �� �ִ�. == ������ �ִ� �װ� ���� ������ ���� �� �ִ�. */
			if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		/* �� �޽ö� �̸��� �Ȱ��� ���� ã�Ƽ� Ǫ���س���. */
		_uint			iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (false == strcmp(m_szName, pBone->Get_Name()))
					return true;

				++iBoneIndex;

				return false;
			});

		/* �� �޽ö� �̸��� ���� ���� �ε���(��ü������)�� �޽þȿ� �����Ѵ�. */
		m_Bones.emplace_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}


	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, vector<class CBone*>& Bones, _fmatrix TransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, Bones, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::NoAiCreate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MESHDESC tDesc, _fmatrix TransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->NoAi_Initialize_Prototype(tDesc, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();
}
