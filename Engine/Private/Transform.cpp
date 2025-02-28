#include "Transform.h"
#include "Shader.h"

#include "Navigation.h"
#include "GameInstance.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix{ rhs.m_WorldMatrix }
{

}


_float3 CTransform::Get_Scaled()
{
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	return _float3(XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_RIGHT])), 
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_UP])), 
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOOK])));
}

void CTransform::Set_Scale(const _float3 & vScale)
{
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	for (size_t i = 0; i < STATE_POSITION; i++)
		Set_State(STATE(i), XMVector3Normalize(WorldMatrix.r[i]) * ((_float*)&vScale)[i]);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_SaveWorldMatrix, XMLoadFloat4x4(&m_WorldMatrix));

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC*		pTransformDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pTransformDesc->fSpeedPerSec;
	m_fRotationPerSec = pTransformDesc->fRotationPerSec;

	return S_OK;
}

void CTransform::Go_InitXYZ(_float fX, _float fY, _float fZ)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vLook = Get_State(STATE_LOOK);
	_vector	vRight = Get_State(STATE_RIGHT);
	_vector vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fX;
	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fY;
	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fZ;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_float3 vNormal = {};
	_int	iNeighborIndex = { -1 };
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	/* isMove() : 이대로 움직여도 되냐? */
	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, &iNeighborIndex, &vNormal))
		Set_State(STATE_POSITION, vPosition);
	else
		if(-2 != iNeighborIndex)
			MoveSlidingV(fTimeDelta, XMVector3Normalize(vLook), XMVector3Normalize(XMLoadFloat3(&vNormal)), pNavigation);
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_float3 vNormal = {};
	_int iNeighborIndex = {};
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, &iNeighborIndex, &vNormal))
		Set_State(STATE_POSITION, vPosition);
	else
		if (-2 != iNeighborIndex)
			MoveSlidingV(fTimeDelta, XMVector3Normalize(vRight), XMVector3Normalize(XMLoadFloat3(&vNormal)), pNavigation);
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_float3 vNormal = {};
	_int iNeighborIndex = {};
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, &iNeighborIndex, &vNormal))
		Set_State(STATE_POSITION, vPosition);
	else
		if (-2 != iNeighborIndex)
			MoveSlidingV(fTimeDelta, XMVector3Normalize(vRight), XMVector3Normalize(XMLoadFloat3(&vNormal)), pNavigation);
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_float3 vNormal = {};
	_int iNeighborIndex = {};
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation || true == pNavigation->isMove(vPosition, &iNeighborIndex, &vNormal))
		Set_State(STATE_POSITION, vPosition);
	else
		MoveSlidingV(fTimeDelta, XMVector3Normalize(vLook), XMVector3Normalize(XMLoadFloat3(&vNormal)), pNavigation);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vUp = Get_State(STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector	vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

/* 항상 항등회전(회전이 없는 상태)기준 지정한 각도만큼 회전한다. */
/* Radian = XMConvertToRadians(Degree) */
void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	/* 회전은 항등상태로!, 스케일은 기존 상태로. */
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_vector		vScale, vRotation, vPosition;

	XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&m_WorldMatrix));

	_float3		vScaled = Get_Scaled();

	/* 회전은 항등상태로!, 스케일은 기존 상태로. */
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;
	vRotation = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));

}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	/* 현재 회전 상태를 얻어온다. */
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	// XMVector3TransformNormal
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

_bool CTransform::TurnTarget(_fvector vAxis, _float fTimeDelta, CTransform* pTarget)
{
	/*
		_vector vTargetWorldPos = pTarget->Get_State(CTransform::STATE_POSITION);
	XMVector3TransformCoord(vTargetWorldPos, ViewMat);
	_vector vThisWorldPos = this->Get_State(CTransform::STATE_POSITION);
	XMVector3TransformCoord(vThisWorldPos, ViewMat);

	_float fDistanceX = XMVectorGetX(vThisWorldPos) - XMVectorGetX(vTargetWorldPos);

	_vector dot = XMVector3Dot(targetLook, MyLook);

	if (fDistanceX >= 2)
		Turn({ 0.f, 1.f, 0.f }, fTimeDelta);
	else if (fDistanceX <= -2)
			Turn({ 0.f, -1.f, 0.f }, fTimeDelta);
	else
		return true;
	*/
	//_vector		targetLook = pTarget->Get_State(STATE_POSITION) - Get_State(STATE_POSITION);
	//_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), targetLook);
	//targetLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	//
	//_float3 vScaled = Get_Scaled();
	//
	//vRight = XMVector4Normalize(vRight) * vScaled.x;
	//vLook = XMVector4Normalize(vLook) * vScaled.z;
	//
	//
	_vector targetLook = XMVector3Normalize(pTarget->Get_State(STATE_LOOK));
	_vector MyLook = XMVector3Normalize(this->Get_State(STATE_LOOK));

	_vector dot = XMVector3Dot(targetLook, MyLook);

	if (XMVectorGetX(dot) >= -0.9)
		Turn(vAxis, fTimeDelta);
	else
		return true;

	return false;
}

_bool CTransform::TurnTarget(_fvector vAxis, _float fTimeDelta, _float4x4 mat)
{

	_vector		targetLook = XMLoadFloat4((_float4*)&mat.m[0][0]) - Get_State(STATE_POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), targetLook);
	targetLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	_float3 vScaled = Get_Scaled();

	vRight = XMVector4Normalize(vRight) * vScaled.x;
	targetLook = XMVector4Normalize(targetLook) * vScaled.z;

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(targetLook) * vScaled.z);


	targetLook = XMVector3Normalize(targetLook);
	_vector MyLook = XMVector3Normalize(this->Get_State(STATE_LOOK));

	_vector dot = XMVector3Dot(targetLook, MyLook);

	if (XMVectorGetX(dot) >= -0.95)
		Turn(vAxis, fTimeDelta);
	else
		return true;

	return false;
}

_bool CTransform::TurnTargetYN(_fvector vAxis, _float fTimeDelta, CTransform* pTarget)
{
	_vector targetLook = XMVector3Normalize(pTarget->Get_State(STATE_LOOK));
	_vector MyLook = XMVector3Normalize(this->Get_State(STATE_LOOK));

	_vector dot = XMVector3Dot(targetLook, MyLook);

	if (XMVectorGetX(dot) >= -0.95)
		return true;
	else
		return false;
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(STATE_POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::LookAtForLandObject(_fvector vTargetPos)
{
	_vector		vLook = vTargetPos - Get_State(STATE_POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	_float3 vScaled = Get_Scaled();

	vRight = XMVector4Normalize(vRight) * vScaled.x;
	vLook = XMVector4Normalize(vLook) * vScaled.z;

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Go_Target(_float fTimeDelta, _fvector vTargetPos, CNavigation* pNavigation)
{
	LookAtForLandObject(vTargetPos);

	_int iNeighborIndex = {};
	_vector vPos = Get_State(STATE_POSITION);
	_vector vDir = vTargetPos - vPos;
	
	_vector fDistance = XMVector3Length(vDir);
	
	if (XMVectorGetX(fDistance) > 0.001f)
	{
		vPos += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;
	}

	// 내가 슬라이딩 하면서 가는 곳에 이웃이 있을 경우에만 이동
	if (nullptr == pNavigation || true == pNavigation->isMove(vPos, &iNeighborIndex))
		Set_State(STATE_POSITION, vPos);
}

void CTransform::Go_TargetDir(_float fTimeDelta, _fvector vDir)
{
	//LookAtForLandObject(vTargetPos);

	_vector vPos = Get_State(STATE_POSITION);

	vPos += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;
	
	Set_State(STATE_POSITION, vPos);
}

_bool CTransform::Go_DistanceCheck(const _fvector vTargetPos, _float fMargin)
{
	_vector vDir = vTargetPos - Get_State(STATE_POSITION);

	_vector fDistance = XMVector3Length(vDir);

	if (XMVectorGetX(fDistance) > fMargin)
		return false;

	return true;
}

_bool CTransform::Go_SavedWorldMatrix(_float fTimeDelta)
{
	_vector TargetPos = *(_vector*)&m_SaveWorldMatrix.m[3][0];
	_vector	vPosition = Get_State(STATE_POSITION);
	_vector vDir = TargetPos - vPosition;

	_float3 vLen;
	XMStoreFloat3(&vLen, XMVector3Length(vDir));

	if (vLen.x < 0.1f)
		return false;
		
	vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;
	Set_State(STATE_POSITION, vPosition);
	return true;
}

void CTransform::MoveSlidingV(_float fTimeDelta, _fvector vMoveVector, _fvector vContactNormal, CNavigation* pNavigation)
{
	//슬라이딩벡터 : D3DXVECTOR3 slidingVector = moveVec - contactNormal * D3DXVec3Dot(&moveVec, &contactNormal);
	_int iNeighborIndex = -1;
	_vector vPosition = Get_State(STATE_POSITION);
	XMVECTOR slidingVector = vMoveVector - vContactNormal * XMVectorGetX(XMVector3Dot(vMoveVector, vContactNormal));
	vPosition += XMVector3Normalize(slidingVector) * m_fSpeedPerSec * fTimeDelta;

	// 내가 슬라이딩 하면서 가는 곳에 이웃이 있을 경우에만 이동
	if(true == pNavigation->isMove(vPosition, &iNeighborIndex))
		Set_State(STATE_POSITION, { XMVectorGetX(vPosition), 1.f, XMVectorGetZ(vPosition), 1.f });
}

void CTransform::Turn_Target(_float fTimeDelta, _fvector vTargetPos)
{
	// cos = normalize(나의 룩벡터) dot normalize(타겟 룩벡터의 역방향)
	// Turn()
}

void CTransform::Set_Billboard()
{
	_float4x4 ViewMatrix;
	_matrix ViewMat = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	XMStoreFloat4x4(&ViewMatrix, ViewMat);

	_float3		vScale = Get_Scaled();
	_float3		vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3		vUp = *(_float3*)&ViewMatrix.m[1][0];
	_float3		vLook = *(_float3*)&ViewMatrix.m[2][0];

	vRight = { vRight.x * vScale.x,  vRight.y * vScale.x, vRight.z * vScale.x };
	vLook = { vLook.x * vScale.z, vLook.y * vScale.z, vLook.z * vScale.z};
	Set_State(CTransform::STATE_RIGHT, XMLoadFloat3(&vRight));
	Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&vLook));
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);	
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CTransform::Clone(void * pArg)
{
	return nullptr;
}

void CTransform::Free()
{
	__super::Free();
}
