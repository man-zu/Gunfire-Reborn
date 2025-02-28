#include "Picking.h"
#include "Transform.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY )
{
    m_hWnd = hWnd;

    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;


    /* 1. 셰이더에서 픽킹이 필요한 픽셀들의 깊이를 렌더타겟에 저장해준다. */
    /* 2. 렌더타겟텍스쳐로부터 깊이를 꺼내와서 월드위치를 계산한다. 단, 렌더타겟텍스쳐에서부터 직접 저장된 값을 꺼내올 수 없다.(D3D11_USAGE_DEFAULT)  */
    /* 3. 위의 렌더타겟을 다른 텍스쳐에 복사하고 복사받은 텍스쳐를 락언락해서 값을 꺼내와야지. */

    /* 3번에 해당하는 복사받을 텍스쳐를 미리 생성하여 준비한다. */
    D3D11_VIEWPORT			ViewportDesc{};
    _uint					iNumViewports = { 1 };
    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    D3D11_TEXTURE2D_DESC		TextureDesc{};

    TextureDesc.Width = (_uint)ViewportDesc.Width;
    TextureDesc.Height = (_uint)ViewportDesc.Height;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_STAGING;
    TextureDesc.BindFlags = 0;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pPickedDepthTexture)))
        return E_FAIL;

    m_fViewWidth = ViewportDesc.Width;
    m_fViewHeight = ViewportDesc.Height;
    
    return S_OK;
}

void CPicking::Tick()
{
    if (CInput_Device::PUSH != m_pGameInstance->Get_DIKeyState(DIK_Z))
        return;

	POINT			ptMouse;
    
	GetCursorPos(&ptMouse);
    
	ScreenToClient(m_hWnd, &ptMouse);
    //
	//XMFLOAT3			vMousePos;
    //
	///* 투영공간상의 마우스 좌표를 구한다. */
	//vMousePos.x = _float(ptMouse.x / (m_iWinSizeX * 0.5f) - 1);
	//vMousePos.y = _float(ptMouse.y / (m_iWinSizeY * -0.5f) + 1);
	//vMousePos.z = 0.f;
    //
	///* 뷰스페이스 상의 좌표를 구한다. */
	//_float4x4		ProjMatrixInv;
	//XMStoreFloat4x4(&ProjMatrixInv, XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ)));
	//XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), XMLoadFloat4x4(&ProjMatrixInv)));
    //
	///* 뷰스페이스 상의 마우스 레이와 레이포스를 구하자. */
	//m_vRayPos = _float3(0.f, 0.f, 0.f);
	//XMStoreFloat3(&m_vRayDir, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&m_vRayPos));
    //
	///* 월드스페이스 상의 마우스 레이와 레이포스를 구하자. */
	//_float4x4		ViewMatrixInv;
	//XMStoreFloat4x4(&ViewMatrixInv, XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))) ;
    //XMStoreFloat3(&m_vRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), XMLoadFloat4x4(&ViewMatrixInv)));
    //XMStoreFloat3(&m_vRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), XMLoadFloat4x4(&ViewMatrixInv)));
    //
    //return;

    /********************************************************************************************************************************/

    /* 픽킹용 깊이를 기록해놓은 렌더타겟을 내가 열수있는 텍스쳐에 복사해준다. */
    /* 텍스쳐를 열어서 깊이를 얻어오는 작업이 가능해진거다. */
    if (FAILED(m_pGameInstance->Copy_RT_Resource(TEXT("Target_PickedDepth"), m_pPickedDepthTexture)))
        return;

    GetCursorPos(&ptMouse);

    /* 뷰포트 상의 마우스 위치를 구했다. */
    /* 1. m_pPickedDepthTexture텍스쳐를 열어서 픽킹용 픽셀의 깊이를 얻어오는 작업을 할꺼야. */
    /* 2. 이픽셀은 몇개? (1280 * 720)이 만큼 있다.== 굉장히 많다. 마우스커서가 있는 위치의 픽셀 깊이하나를 얻어오고싶다. */
    /* 3. . 마우스커서가 있는 위치의 픽셀 깊이하나를 얻어오고싶다. ==
    _uint		iIndex = ptMouse.y * m_fViewWidth + ptMouse.x;*/
    /* 3. 위의 연산식은 언제썼떤거여? api때, 마우스 위치에 해당하는 타일의 인덱스 구하는 연산. */


    /* 1. 지금 구한 뷰포트 상의 마우스위치를 통해 월드스페이스 상의 마우스 위치를 구하기위한 과정에 활용이된다. */
    ScreenToClient(m_hWnd, &ptMouse);

    if (ptMouse.x < (_long)0.f)
        ptMouse.x = (_long)0.f;
    if (ptMouse.x > (_long)m_fViewWidth)
        ptMouse.x = (_long)m_fViewWidth;

    if (ptMouse.y < (_long)0.f)
        ptMouse.y = (_long)0.f;
    if (ptMouse.y > (_long)m_fViewHeight)
        ptMouse.y = (_long)m_fViewHeight;


    D3D11_MAPPED_SUBRESOURCE		SubResource{};


    /* 피킹용 깊이를 저장해놓은 테긋쳐를 연다! */
    m_pContext->Map(m_pPickedDepthTexture, 0, D3D11_MAP_READ, 0, &SubResource);


    _uint		iIndex = _uint(ptMouse.y * m_fViewWidth + ptMouse.x);

    /* 마우스 있는 위치의 픽셀 정보를 얻어오낟. */
    /* Out.vPickedDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f); */

    _float4		vDepth = ((_float4*)SubResource.pData)[iIndex];

    m_pContext->Unmap(m_pPickedDepthTexture, 0);

    if (1.f == vDepth.w)
        return;

    /* ptMouse.x, ptMouse.y => 뷰포트 상의 위치다. 월드까지 역으로 연산을 해나가야한다. */

    /* 뷰포트상의 위치(0, 0) ~ (wsx, wsy) -> 투영스페이스 상의 위치(-1, 1) ~ (1, -1)를 구한다. */
    m_vPickPos.x = ptMouse.x / (m_fViewWidth * 0.5f) - 1.f;
    m_vPickPos.y = ptMouse.y / (m_fViewHeight * -0.5f) + 1.f;
    m_vPickPos.z = vDepth.x;
    m_vPickPos.w = 1.f;

    /* 투영공간상의 위치 -> 뷰스페이스 상의 위치를 구한다. */
    /* 역투영행렬을 바로 곱한 이유? -> 쉐이더에서 했다라면 바로 역투영행렬을 곱하면 안되었다. 나눴던 w를 곱해서 원근투영을 복구해주는 작업이 필요했엇다. */
    /* 쉐이더가아니라 클라. TransformCoord함수는 알아서 연산에 대한 처리를 해준다. */
    _vector		vPickPos = XMVector3TransformCoord(XMLoadFloat4(&m_vPickPos), m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ));

    /* 뷰스페이스상의 위치 -> 월드 상의 위치를 구한다. */
    vPickPos = XMVector3TransformCoord(vPickPos, m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW));

    // 파트오브젝트는 부모행렬과 곱한 월드를 가져와야하는데,,

    XMStoreFloat4(&m_vPickPos, vPickPos);
}

void CPicking::Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, _float4x4* pWorldMat)
{
	// 피킹하는 대상의 월드역행렬로 로컬스페이스로 전환
	XMStoreFloat3(pRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), XMMatrixInverse(nullptr, XMLoadFloat4x4(pWorldMat))));
	XMStoreFloat3(pRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), XMMatrixInverse(nullptr, XMLoadFloat4x4(pWorldMat))));
}

_bool CPicking::IntersectTri(const XMVECTOR& rayPos, const XMVECTOR& rayDir, const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2, float* t, float* u, float* v)
{
    // 삼각형 두 변 계산
    XMVECTOR edge1 = v1 - v0;
    XMVECTOR edge2 = v2 - v0;

    // 평면의 법선 벡터 계산. 외적하면 법선벡터
    XMVECTOR pvec = XMVector3Cross(rayDir, edge2);

    // 법선벡터랑 삼각형 선분과 내적. 행렬식 계산
    float det = XMVectorGetX(XMVector3Dot(edge1, pvec));

    // 역행렬식 계산
    XMVECTOR inv_det = XMVectorReciprocal(XMVectorReplicate(det));

    // 광선의 시작점과 삼각형의 첫 번째 꼭짓점 간의 벡터를 구함
    XMVECTOR tvec = rayPos - v0;

    // U 파라미터를 계산하고 검증
    XMVECTOR uvec = XMVector3Dot(tvec, pvec);
    *u = XMVectorGetX(uvec * inv_det);
    if (*u < 0.0f || *u > 1.0f)
        return false;

    // V 파라미터를 계산하고 검증
    XMVECTOR qvec = XMVector3Cross(tvec, edge1);

    // 교차 거리를 계산
    XMVECTOR vvec = XMVector3Dot(rayDir, qvec);
    *v = XMVectorGetX(vvec * inv_det);
    if (*v < 0.0f || *u + *v > 1.0f)
        return false;

    XMVECTOR tvec2 = XMVector3Dot(edge2, qvec);
    *t = XMVectorGetX(tvec2 * inv_det);

    _float3 fPos, fDir;
    XMStoreFloat3(&fPos, rayPos);
    XMStoreFloat3(&fDir, rayDir);

    return true;
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    CPicking* pInstance = new CPicking(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
    {
        MSG_BOX(L"Failed To Created CPicking");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPicking::Free()
{
    __super::Free();

    Safe_Release(m_pPickedDepthTexture);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
