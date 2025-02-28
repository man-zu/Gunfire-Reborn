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


    /* 1. ���̴����� ��ŷ�� �ʿ��� �ȼ����� ���̸� ����Ÿ�ٿ� �������ش�. */
    /* 2. ����Ÿ���ؽ��ķκ��� ���̸� �����ͼ� ������ġ�� ����Ѵ�. ��, ����Ÿ���ؽ��Ŀ������� ���� ����� ���� ������ �� ����.(D3D11_USAGE_DEFAULT)  */
    /* 3. ���� ����Ÿ���� �ٸ� �ؽ��Ŀ� �����ϰ� ������� �ؽ��ĸ� ������ؼ� ���� �����;���. */

    /* 3���� �ش��ϴ� ������� �ؽ��ĸ� �̸� �����Ͽ� �غ��Ѵ�. */
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
	///* ������������ ���콺 ��ǥ�� ���Ѵ�. */
	//vMousePos.x = _float(ptMouse.x / (m_iWinSizeX * 0.5f) - 1);
	//vMousePos.y = _float(ptMouse.y / (m_iWinSizeY * -0.5f) + 1);
	//vMousePos.z = 0.f;
    //
	///* �佺���̽� ���� ��ǥ�� ���Ѵ�. */
	//_float4x4		ProjMatrixInv;
	//XMStoreFloat4x4(&ProjMatrixInv, XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ)));
	//XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), XMLoadFloat4x4(&ProjMatrixInv)));
    //
	///* �佺���̽� ���� ���콺 ���̿� ���������� ������. */
	//m_vRayPos = _float3(0.f, 0.f, 0.f);
	//XMStoreFloat3(&m_vRayDir, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&m_vRayPos));
    //
	///* ���彺���̽� ���� ���콺 ���̿� ���������� ������. */
	//_float4x4		ViewMatrixInv;
	//XMStoreFloat4x4(&ViewMatrixInv, XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))) ;
    //XMStoreFloat3(&m_vRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), XMLoadFloat4x4(&ViewMatrixInv)));
    //XMStoreFloat3(&m_vRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), XMLoadFloat4x4(&ViewMatrixInv)));
    //
    //return;

    /********************************************************************************************************************************/

    /* ��ŷ�� ���̸� ����س��� ����Ÿ���� ���� �����ִ� �ؽ��Ŀ� �������ش�. */
    /* �ؽ��ĸ� ��� ���̸� ������ �۾��� ���������Ŵ�. */
    if (FAILED(m_pGameInstance->Copy_RT_Resource(TEXT("Target_PickedDepth"), m_pPickedDepthTexture)))
        return;

    GetCursorPos(&ptMouse);

    /* ����Ʈ ���� ���콺 ��ġ�� ���ߴ�. */
    /* 1. m_pPickedDepthTexture�ؽ��ĸ� ��� ��ŷ�� �ȼ��� ���̸� ������ �۾��� �Ҳ���. */
    /* 2. ���ȼ��� �? (1280 * 720)�� ��ŭ �ִ�.== ������ ����. ���콺Ŀ���� �ִ� ��ġ�� �ȼ� �����ϳ��� ������ʹ�. */
    /* 3. . ���콺Ŀ���� �ִ� ��ġ�� �ȼ� �����ϳ��� ������ʹ�. ==
    _uint		iIndex = ptMouse.y * m_fViewWidth + ptMouse.x;*/
    /* 3. ���� ������� �����趲�ſ�? api��, ���콺 ��ġ�� �ش��ϴ� Ÿ���� �ε��� ���ϴ� ����. */


    /* 1. ���� ���� ����Ʈ ���� ���콺��ġ�� ���� ���彺���̽� ���� ���콺 ��ġ�� ���ϱ����� ������ Ȱ���̵ȴ�. */
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


    /* ��ŷ�� ���̸� �����س��� �ױ��ĸ� ����! */
    m_pContext->Map(m_pPickedDepthTexture, 0, D3D11_MAP_READ, 0, &SubResource);


    _uint		iIndex = _uint(ptMouse.y * m_fViewWidth + ptMouse.x);

    /* ���콺 �ִ� ��ġ�� �ȼ� ������ ������. */
    /* Out.vPickedDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f); */

    _float4		vDepth = ((_float4*)SubResource.pData)[iIndex];

    m_pContext->Unmap(m_pPickedDepthTexture, 0);

    if (1.f == vDepth.w)
        return;

    /* ptMouse.x, ptMouse.y => ����Ʈ ���� ��ġ��. ������� ������ ������ �س������Ѵ�. */

    /* ����Ʈ���� ��ġ(0, 0) ~ (wsx, wsy) -> ���������̽� ���� ��ġ(-1, 1) ~ (1, -1)�� ���Ѵ�. */
    m_vPickPos.x = ptMouse.x / (m_fViewWidth * 0.5f) - 1.f;
    m_vPickPos.y = ptMouse.y / (m_fViewHeight * -0.5f) + 1.f;
    m_vPickPos.z = vDepth.x;
    m_vPickPos.w = 1.f;

    /* ������������ ��ġ -> �佺���̽� ���� ��ġ�� ���Ѵ�. */
    /* ����������� �ٷ� ���� ����? -> ���̴����� �ߴٶ�� �ٷ� ����������� ���ϸ� �ȵǾ���. ������ w�� ���ؼ� ���������� �������ִ� �۾��� �ʿ��߾���. */
    /* ���̴����ƴ϶� Ŭ��. TransformCoord�Լ��� �˾Ƽ� ���꿡 ���� ó���� ���ش�. */
    _vector		vPickPos = XMVector3TransformCoord(XMLoadFloat4(&m_vPickPos), m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ));

    /* �佺���̽����� ��ġ -> ���� ���� ��ġ�� ���Ѵ�. */
    vPickPos = XMVector3TransformCoord(vPickPos, m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW));

    // ��Ʈ������Ʈ�� �θ���İ� ���� ���带 �����;��ϴµ�,,

    XMStoreFloat4(&m_vPickPos, vPickPos);
}

void CPicking::Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, _float4x4* pWorldMat)
{
	// ��ŷ�ϴ� ����� ���忪��ķ� ���ý����̽��� ��ȯ
	XMStoreFloat3(pRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), XMMatrixInverse(nullptr, XMLoadFloat4x4(pWorldMat))));
	XMStoreFloat3(pRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), XMMatrixInverse(nullptr, XMLoadFloat4x4(pWorldMat))));
}

_bool CPicking::IntersectTri(const XMVECTOR& rayPos, const XMVECTOR& rayDir, const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2, float* t, float* u, float* v)
{
    // �ﰢ�� �� �� ���
    XMVECTOR edge1 = v1 - v0;
    XMVECTOR edge2 = v2 - v0;

    // ����� ���� ���� ���. �����ϸ� ��������
    XMVECTOR pvec = XMVector3Cross(rayDir, edge2);

    // �������Ͷ� �ﰢ�� ���а� ����. ��Ľ� ���
    float det = XMVectorGetX(XMVector3Dot(edge1, pvec));

    // ����Ľ� ���
    XMVECTOR inv_det = XMVectorReciprocal(XMVectorReplicate(det));

    // ������ �������� �ﰢ���� ù ��° ������ ���� ���͸� ����
    XMVECTOR tvec = rayPos - v0;

    // U �Ķ���͸� ����ϰ� ����
    XMVECTOR uvec = XMVector3Dot(tvec, pvec);
    *u = XMVectorGetX(uvec * inv_det);
    if (*u < 0.0f || *u > 1.0f)
        return false;

    // V �Ķ���͸� ����ϰ� ����
    XMVECTOR qvec = XMVector3Cross(tvec, edge1);

    // ���� �Ÿ��� ���
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
