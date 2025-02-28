#include "ImGui_Manager.h"

#include "Input_Device.h"
#include "GameInstance.h"

#include "UI_Crosshair.h"
#include "Camera.h"
#include "Level.h"

#include "Terrain.h"
#include "Obj_Common.h"

#include <json/nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

IMPLEMENT_SINGLETON(CImGui_Manager)

_int		CImGui_Manager::iCntIdx = 0;

CImGui_Manager::CImGui_Manager()
    :m_pGameInstance(CGameInstance::Get_Instance())
{
}

HRESULT CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd)
{
    m_pDevice = pDevice;
    m_pContext = pContext;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.Fonts->AddFontFromFileTTF("../Bin/Resources/Fonts/Hancom Gothic Regular.ttf", 16.f, nullptr, io.Fonts->GetGlyphRangesKorean());

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

	return S_OK;
}

void CImGui_Manager::Tick()
{
    if(m_pNavigationCom == nullptr)
    {
        if (GET_CURLEVEL == LEVEL_STAGE1)
        {
            /* Add m_pNavigationCom */
            CTerrain* pTerrain = dynamic_cast<CTerrain*>(GET_LIST(LEVEL_STAGE1, L"Layer_BackGround")->front());
            m_pNavigationCom = pTerrain->GetNaviUsingTool();
            Safe_AddRef(m_pNavigationCom);
        }
        if (GET_CURLEVEL == LEVEL_STAGE1_BOSS)
        {
            /* Add m_pNavigationCom */
            CTerrain* pTerrain = dynamic_cast<CTerrain*>(GET_LIST(LEVEL_STAGE1_BOSS, L"Layer_BackGround")->front());
            m_pNavigationCom = pTerrain->GetNaviUsingTool();
            Safe_AddRef(m_pNavigationCom);
        }
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    GuizmoBeginSet();

    static _bool bStart = { false };
    static _float fRotY = { 0 };

    if (KEY_PUSH(DIK_F9))
    {
        bStart = !bStart;

        if (LEVEL_STAGE1 == GET_CURLEVEL)
        {

            //list<class CGameObject*>* pObjMouse = GET_LIST(LEVEL_STAGE1, L"Layer_Crosshair");
            //
            //if (pObjMouse->size() <= 0)
            //    return;
            //
            //if (m_bMoseMove)
            //    dynamic_cast<CUI_Crosshair*>(pObjMouse->front())->SetFree(true);
            //else
            //    dynamic_cast<CUI_Crosshair*>(pObjMouse->front())->SetFree(false);
        }
    }

    if (KEY_PUSH(DIK_LCONTROL))
    {

        m_bMoseMove = !m_bMoseMove;
        LEVEL curLevel = GET_CURLEVEL;

        if (curLevel == LEVEL_STAGE1 || curLevel == LEVEL_STAGE1_BOSS)
        {
            list<class CGameObject*>* pObjCam = GET_LIST(curLevel, L"Layer_MainCamera");

            if (m_bMoseMove)
            {
                dynamic_cast<CCamera*>(pObjCam->front())->SetMouseSensorUse(true);
            }
            else
            {
                dynamic_cast<CCamera*>(pObjCam->front())->SetMouseSensorUse(false);
            }
        }
    }

    if (KEY_PUSH(DIK_1))
    {
        m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;
    }
    else if (KEY_PUSH(DIK_2))
    {
        m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;
    }
    else if (KEY_PUSH(DIK_3))
    {
        m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;
    }

    if (KEY_PUSH(DIK_SLASH))
    {
        if (LEVEL_STAGE1 == GET_CURLEVEL)
            dynamic_cast<CObj_Common*>(m_pSelectObj)->setRotate(fRotY);
    }




    if (true == bStart)
    {
        ImGui::Begin("==========");
        /* ****************** [ Object Tab ] ******************
        * LOAD MODEL
        * protoTypeTag 매치시키기
        * position
        */

        static _char str0[128] = "";
        static _float4 iPos = { 0.f, 0.f, 0.f, 1.f };
        static _int iNorX = { 0 };
        static _int iNorY = { 0 };
        static _int iNorZ = { 0 };

        static _bool bNavi = false;
        static _bool bHideNavi = false;
        static _bool bPicked = false;
        static _bool bIsChgSelected = false;

        static _int item_current_idx = 0; // Here we store our selection data as an index.

        if(bPicked)
            PickAdd(item_current_idx);

        if (bNavi)
            DrawPoints();

        if(GET_CURLEVEL == LEVEL_STAGE1)
            HideSavedNavi(!bHideNavi);


        // Tabs
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {

            /********************************************************************************************/
            /****************************************** Object ******************************************/
            /********************************************************************************************/

            if (ImGui::BeginTabItem("Object"))
            {
                ImGui::Text(u8" Object 배치 ");

                /* ************ Object Setting ( LIST ) ************ */
                ImGui::SeparatorText("LOAD Object File");
                ImGui::Text(u8" Object 수정 후 반드시 SAVE 할 것 !");
                
                if (ImGui::Button("LOAD"))
                    LoadObjDatas();
                ImGui::SameLine();

                if (ImGui::Button("SAVE"))
                    SaveObjDatas();
                ImGui::SameLine();

                if (ImGui::Button("TEST"))
                    test();
                ImGui::SameLine();

                if (ImGui::Button("+"))
                    LoadObjItem();
                ImGui::SameLine();

                if (ImGui::Button("-"))
                    RemoveData(item_current_idx);
                ImGui::SameLine();
                ImGui::Checkbox("Picking Add", &bPicked);

                if (ImGui::BeginListBox("##listbox"))
                {
                    for (int n = 0; n < m_tOBJDatas.size(); n++)
                    {
                        const bool is_selected = (item_current_idx == n);
                        if (ImGui::Selectable(m_tOBJDatas[n]->sLayerTag.c_str(), is_selected))
                        {
                            item_current_idx = n;
                            bIsChgSelected = true;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();

                            wstring sLayerTag = L"";
                            sLayerTag.assign(m_tOBJDatas[item_current_idx]->sLayerTag.begin(), m_tOBJDatas[item_current_idx]->sLayerTag.end());
                            
                            /* 선택한 객체 찾아서 m_pSelectObj 에 담기 */
                            m_pSelectObj = GET_LIST(LEVEL_STAGE1, sLayerTag)->front();
                            
                            size_t tmp = strnlen_s(str0, MAXLEN);

                            if (bIsChgSelected || tmp + 0 == 0)
                            {
                                bIsChgSelected = false;
                                strcpy_s(str0, m_tOBJDatas[item_current_idx]->sLayerTag.c_str());
                                iPos.x = m_tOBJDatas[item_current_idx]->fPosX;
                                iPos.y = m_tOBJDatas[item_current_idx]->fPosY;
                                iPos.z = m_tOBJDatas[item_current_idx]->fPosZ;
                                fRotY = m_tOBJDatas[item_current_idx]->fRotY;
                            }
                            
                            UseGuizmo(&iPos);
                        }             
                    }                 
                    ImGui::EndListBox();
                }

                /* ************ Object Setting ( ITEM ) ************ */ 
                ImGui::SeparatorText("Object Setting");
                ImGui::Text("NameTag : "); ImGui::SameLine();
                ImGui::InputText("##prototypeTag", str0, IM_ARRAYSIZE(str0));

                ImGui::Text("position : "); ImGui::SameLine();
                ImGui::PushItemWidth(50.f);
                ImGui::InputFloat("##ObjPosX", &iPos.x, 0, 0, 0);
                ImGui::SameLine();
                ImGui::PushItemWidth(50.f);
                ImGui::InputFloat("##ObjPosY", &iPos.y, 0, 0, 0);
                ImGui::SameLine();
                ImGui::PushItemWidth(50.f);
                ImGui::InputFloat("##ObjPosZ", &iPos.z, 0, 0, 0);

                ImGui::Text(u8"Y축 회전 : "); ImGui::SameLine();
                ImGui::PushItemWidth(50.f);
                ImGui::InputFloat("##fRotY", &fRotY, 0, 0, 0);

                const float ItemSpacing = ImGui::GetStyle().ItemSpacing.x;
                static float HostButtonWidth = 100.0f; //The 100.0f is just a guess size for the first frame.
                float pos = HostButtonWidth + ItemSpacing;
                ImGui::SameLine(ImGui::GetWindowWidth() - pos);
                ImVec2 buttonSize(60, 30); // My button size
                if (ImGui::Button("Save", buttonSize))
                {
                    SaveObjItem(item_current_idx, &iPos, fRotY);
                }


                ImGui::EndTabItem();
            }

            /********************************************************************************************/
            /**************************************** Navigation ****************************************/
            /********************************************************************************************/

            if (ImGui::BeginTabItem("Navigation"))
            {
                ImGui::Text(u8" Navigation 삼각형 그리기 ");

                ImGui::Checkbox("Draw Navi", &bNavi);
                ImGui::Checkbox("Hide Navi", &bHideNavi);

                if (ImGui::Button("LOAD"))
                    LoadNavi();
                ImGui::SameLine();
                if (ImGui::Button("SAVE"))
                    SaveNavi();
                ImGui::SameLine();
                if (ImGui::Button(u8"실행취소"))
                    ReturnNavi();
                ImGui::SameLine();


                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }
    ImGui::EndFrame();

}

HRESULT CImGui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

#ifdef _DEBUG
    //if(nullptr != m_pNavigationCom)
    //    m_pNavigationCom->Render();
#endif

	return S_OK;
}

void CImGui_Manager::GuizmoBeginSet()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::SetOrthographic(false);
}

void CImGui_Manager::SaveObjItem(_uint idx, _float4* _pos, _float _rotY)
{
    m_tOBJDatas[idx]->eObjType = CObject::OBJ_MAP;
    m_tOBJDatas[idx]->fPosX = _pos->x;
    m_tOBJDatas[idx]->fPosY = _pos->y;
    m_tOBJDatas[idx]->fPosZ = _pos->z;
    m_tOBJDatas[idx]->fRotY = _rotY;
    m_tOBJDatas[idx]->fX = 0.f;
    m_tOBJDatas[idx]->fY = 0.f;

    MSG_BOX(L"Success!");

    return;
}

void CImGui_Manager::LoadObjDatas()
{
    //OPENFILENAME Ofn;
    //ZeroMemory(&Ofn, sizeof OPENFILENAME);
    //
    //_tchar szFilePath[MAX_PATH];
    //ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);
    //static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0*.json\0fbx 파일\0*.fbx";
    //
    //Ofn.lStructSize = sizeof OPENFILENAME;
    //Ofn.hwndOwner = g_hWnd;
    //Ofn.lpstrFile = szFilePath;
    //Ofn.lpstrFilter = filter;
    //Ofn.nMaxFile = MAX_PATH;
    //Ofn.lpstrInitialDir = L"..\\Bin\\ToolSavedFiles\\Object\\";
    //Ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    //
    //_int iRet = { 0 };
    //iRet = GetOpenFileName(&Ofn);
    //
    //if (0 == iRet || L"" == Ofn.lpstrFile)
    //{
    //    MSG_BOX(L"Failed To Load File");
    //    return;
    //}
    //
    //_ulong dwByte = { 0 };
    //HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    //
    //if (INVALID_HANDLE_VALUE == hFile)
    //{
    //    MSG_BOX(L"Can't Open File To Load");
    //    return;
    //}

    try {
        std::ifstream file("../Bin/ToolSavedFiles/obj/stage2.json", std::ios::in);
        //std::ifstream file("../Bin/ToolSavedFiles/test/saveObject.dat", std::ios::in);
        if (!file.is_open())
        {
            std::cout << "Error opening testMap.json.\n";
            return;
        }
        json jDataList;
        file >> jDataList;

        file.close();


        CObject::OBJDESC* pDesc = {};

        for (auto& jData : jDataList)
        {
            pDesc = new CObject::OBJDESC;

            CObject::OBJDESC transDesc{};

            pDesc->sFileName = jData["sFileName"];
            pDesc->sLayerTag = jData["sLayerTag"];
            pDesc->sPrototpyeName = jData["sPrototpyeName"];
            pDesc->sComponentName = transDesc.sComponentName = jData["sComponentName"];
            pDesc->fPosX = transDesc.fPosX = jData["fPosX"];
            pDesc->fPosY = transDesc.fPosY = jData["fPosY"];
            pDesc->fPosZ = transDesc.fPosZ = jData["fPosZ"];
            pDesc->fRotY = transDesc.fRotY = jData["fRotY"];

            m_tOBJDatas.push_back(pDesc);

            wstring sLayerTag = L"";
            sLayerTag.assign(pDesc->sLayerTag.begin(), pDesc->sLayerTag.end());
            wstring sPrototypeName = L"";
            sPrototypeName.assign(pDesc->sPrototpyeName.begin(), pDesc->sPrototpyeName.end());

            transDesc.fSpeedPerSec = 10.f;
            transDesc.fRotationPerSec = XMConvertToRadians(90.0f);

            if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1, sLayerTag, sPrototypeName, &transDesc)))
                return;
        }
        
        return;
    }
    catch (std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return;
    }

}

void CImGui_Manager::SaveObjDatas()
{
    /* 파일 경로 얻기 위한 작업,, */
    OPENFILENAME Ofn;
    ZeroMemory(&Ofn, sizeof OPENFILENAME);

    _tchar szFilePath[MAX_PATH];
    ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

    Ofn.lStructSize = sizeof OPENFILENAME;
    Ofn.hwndOwner = g_hWnd;
    Ofn.hInstance = NULL;
    Ofn.lpstrFile = szFilePath;
    Ofn.lpstrFilter = NULL;
    Ofn.nMaxFile = MAX_PATH;
    Ofn.lpstrDefExt = L"dat";
    Ofn.lpstrInitialDir = L"..\\Bin\\ToolSavedFiles\\Object\\";
    Ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    _int iRet = { 0 };
    iRet = GetSaveFileName(&Ofn);

    if (0 == iRet || L"" == Ofn.lpstrFile)
    {
        MSG_BOX(L"Failed To Save File");
        return;
    }

    /* JSON 타입으로 파일 저장 */
    wstring tmp(&szFilePath[0]);
    string savePath(tmp.begin(), tmp.end());
    SaveJSON(ParsingJSON(), savePath);

}

void CImGui_Manager::LoadObjItem()
{
    OPENFILENAME Ofn;
    ZeroMemory(&Ofn, sizeof OPENFILENAME);

    _tchar szFilePath[MAX_PATH];
    ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);
    static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0fbx 파일\0*.fbx";

    Ofn.lStructSize = sizeof OPENFILENAME;
    Ofn.hwndOwner = g_hWnd;
    Ofn.lpstrFile = szFilePath;
    Ofn.lpstrFilter = filter;
    Ofn.nMaxFile = MAX_PATH;
    Ofn.lpstrInitialDir = L"..\\Bin\\Resources\\Models\\";
    Ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    _int iRet = { 0 };
    iRet = GetOpenFileName(&Ofn);

    if (0 == iRet || L"" == Ofn.lpstrFile)
    {
        MSG_BOX(L"Failed To Load File");
        return;
    }

    /* Get FileName */
    wchar_t fileName[MAX_PATH];
    wcsncpy_s(fileName, wcsrchr(szFilePath, L'\\'), wcslen(wcsrchr(szFilePath, L'\\'))-4);
    wstring ws(fileName);


    CObject::OBJDESC* pDesc = new CObject::OBJDESC;
    pDesc->fPosX = 0.f;
    pDesc->fPosY = 0.f;
    pDesc->fPosZ = 0.f;
    pDesc->fRotY = 0.f;
    pDesc->fX = 0.f;
    pDesc->fY = 0.f;

    pDesc->sFileName = string(ws.begin() + 1, ws.end());
    pDesc->sLayerTag = "Layer_Obj_" + string(ws.begin() + 1, ws.end()) + to_string(iCntIdx++);
    pDesc->sPrototpyeName = "Prototype_GameObject_Obj_" + string(ws.begin() + 1, ws.end());
    pDesc->sComponentName = "Prototype_Component_Obj_" + string(ws.begin() + 1, ws.end());
    m_tOBJDatas.push_back(pDesc);

    wstring sLayerTag = L"";
    sLayerTag.assign(pDesc->sLayerTag.begin(), pDesc->sLayerTag.end());
    wstring sPrototypeName = L"";
    sPrototypeName.assign(pDesc->sPrototpyeName.begin(), pDesc->sPrototpyeName.end());

    CObject::OBJDESC transDesc{};

    transDesc.fSpeedPerSec = 10.f;
    transDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    transDesc.sComponentName = pDesc->sComponentName;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1, sLayerTag, sPrototypeName, &transDesc)))
        return;


}

void CImGui_Manager::RemoveData(_uint idx)
{
    if (idx >= m_tOBJDatas.size())
        return;

    wstring sLayerTag;
    sLayerTag.assign(m_tOBJDatas[idx]->sLayerTag.begin(), m_tOBJDatas[idx]->sLayerTag.end());
    GET_LIST(LEVEL_STAGE1, sLayerTag)->front()->Set_Dead(true);

    m_tOBJDatas.erase(m_tOBJDatas.begin()+idx);

}

void CImGui_Manager::PickAdd(_uint idx)
{
    if (idx >= m_tOBJDatas.size())
        return;

    // 피킹지점 구하기
    // 해당 지점을 x,y,z 로 세팅
    CTerrain* pTerrain = dynamic_cast<CTerrain*>(GET_LIST(LEVEL_STAGE1, L"Layer_BackGround")->front());

    _float3 vPickPos = { 0.f, 0.f, 0.f };
    pTerrain->GetPickPos(&vPickPos);

    if (vPickPos.x == 0.f)
    {
        return;
    }

    std::cout << vPickPos.x << ", " << vPickPos.y << ", " << vPickPos.z << endl;
    std::cout << "-----------------------------------------------------------" << endl;	// 월드좌표

    /* m_tOBJDatas 오브젝트 추가 */
    CObject::OBJDESC* addObjDesc = new CObject::OBJDESC;
    
    addObjDesc->sFileName = m_tOBJDatas[idx]->sFileName;
    addObjDesc->sLayerTag = "Layer_Obj_" + m_tOBJDatas[idx]->sFileName + to_string(iCntIdx++);;
    addObjDesc->sComponentName = m_tOBJDatas[idx]->sComponentName;
    addObjDesc->sPrototpyeName = m_tOBJDatas[idx]->sPrototpyeName;
    addObjDesc->fPosX = vPickPos.x;
    addObjDesc->fPosY = vPickPos.y;
    addObjDesc->fPosZ = vPickPos.z;
    addObjDesc->fRotY = 0.f;

    m_tOBJDatas.push_back(addObjDesc);

    wstring sLayerTag = L"";
    sLayerTag.assign(addObjDesc->sLayerTag.begin(), addObjDesc->sLayerTag.end());
    wstring sPrototypeName = L"";
    sPrototypeName.assign(addObjDesc->sPrototpyeName.begin(), addObjDesc->sPrototpyeName.end());

    addObjDesc->fSpeedPerSec = 10.f;
    addObjDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE1, sLayerTag, sPrototypeName, addObjDesc)))
        return;
}

void CImGui_Manager::UseGuizmo(_float4* _pos)
{
    // 저장 버튼 안누르면 desc 랑 정보 달라서 파르르르르
    
    // 월드 행렬, 뷰행렬, 투영행렬 받아와서 이용해야함.
    CTransform* pObjTransform = dynamic_cast<CTransform*>(m_pSelectObj->Find_Component(L"Com_Transform"));
    _float4x4 WorldMat, ViewMat, ProjMat;
    //_float4  vPos;
    static _float3 vRot = { 0.f,0.f,0.f };
    _float3 vScale;

    //각 값들 받아서 저장
    WorldMat = *(pObjTransform->Get_WorldFloat4x4());
    m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
    ViewMat = GET_PIPELINE(CPipeLine::D3DTS_VIEW);
    ProjMat = GET_PIPELINE(CPipeLine::D3DTS_PROJ);

    pObjTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(_pos));

    //XMStoreFloat4(_pos, pObjTransform->Get_State(CTransform::STATE_POSITION));
    vScale = pObjTransform->Get_Scaled();

    ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &_pos->x, &vRot.x, &vScale.x);
    ImGuizmo::RecomposeMatrixFromComponents(&_pos->x, &vRot.x, &vScale.x, &WorldMat._11);

    if (ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11	// 뷰, 투영행렬
        , m_tGizmoDesc.CurrentGizmoOperation			// Tr, Rt, Sc
        , m_tGizmoDesc.CurrentGizmoMode				    // WORLD, LOCAL
        , (_float*)&WorldMat					        // 객체의 월드 행렬
        , NULL							                // 그냥 NULL 고정하자
        , m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL	// 위의 틱당 단위 설정들
        , m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
        , m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
    {
        pObjTransform->Set_WorldMat(WorldMat);
    }
}

void CImGui_Manager::test()
{
    int a = 0;
    m_tOBJDatas;

    ParsingJSON();
    LoadJSON();
}

void CImGui_Manager::Rounds(_float& self, _int _iRound)
{
	self = self * pow(10, _iRound + 1) + 5;
	self = (int)self / 10 / pow(10, _iRound);
}

void CImGui_Manager::SetPointsClockWise(_float3* pPoints)
{
    XMFLOAT3 AB = { pPoints[1].x - pPoints[0].x, pPoints[1].y - pPoints[0].y, pPoints[1].z - pPoints[0].z };
    XMFLOAT3 AC = { pPoints[2].x - pPoints[0].x, pPoints[2].y - pPoints[0].y, pPoints[2].z - pPoints[0].z };

    // 벡터 AB와 AC의 외적 계산
    XMVECTOR v1 = XMLoadFloat3(&AB);
    XMVECTOR v2 = XMLoadFloat3(&AC);
    XMVECTOR cross = XMVector3Cross(XMLoadFloat3(&AB), XMLoadFloat3(&AC));

    XMFLOAT3 crossProduct;
    XMStoreFloat3(&crossProduct, cross);

    _float fCrossResult = crossProduct.x + crossProduct.y + crossProduct.z;

    cout << "[ImGui_Manager.cpp] fCrossResult : " << fCrossResult << endl;

    if (fCrossResult < 0)
    {
        cout << "시계 반대방향입니다. " << endl;
        _float3 vTmpPoint;
        vTmpPoint = m_fPoint[2];
        m_fPoint[2] = m_fPoint[1];
        m_fPoint[1] = vTmpPoint;
    }
}

HRESULT CImGui_Manager::LoadNavi()
{
    OPENFILENAME Ofn;
    ZeroMemory(&Ofn, sizeof OPENFILENAME);

    _tchar szFilePath[MAX_PATH];
    ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);
    static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0fbx 파일\0*.fbx";

    Ofn.lStructSize = sizeof OPENFILENAME;
    Ofn.hwndOwner = g_hWnd;
    Ofn.lpstrFile = szFilePath;
    Ofn.lpstrFilter = filter;
    Ofn.nMaxFile = MAX_PATH;
    Ofn.lpstrInitialDir = L"..\\Bin\\ToolSavedFiles\\Navigation\\";
    Ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    _int iRet = { 0 };
    iRet = GetOpenFileName(&Ofn);

    if (0 == iRet || L"" == Ofn.lpstrFile)
    {
        MSG_BOX(L"Failed To Load File");
        return E_FAIL;
    }

    /* 파일읽기 */
	_ulong dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;


    //for (auto& iter : m_vNaviDatas)
    //    Safe_Delete(iter);

    m_vNaviDatas.clear();

    _uint i = 0;

    while (true)
    {
        _float3		vPoints[3] = {};

        ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);


        if (0 == dwByte)
        {
            break;
        }

        /* m_pNavigationCom 보여질 수 있도록 Cell 추가 */
        m_pNavigationCom->AddCell(vPoints[0], vPoints[1]);
        /* 삼각형 시계방향 */
        SetPointsClockWise(vPoints);

        m_pNavigationCom->AddCell(vPoints[1], vPoints[2]);
        m_pNavigationCom->AddCell(vPoints[2], vPoints[0]);


        /* STL Vector 저장 */
        for (int i = 0; i < 3; ++i)
        {
            m_vNaviDatas.push_back(vPoints[i]);
        }

    }

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CImGui_Manager::SaveNavi()
{
    /* 파일저장 */
    _ulong		dwByte = { 0 };
    HANDLE		hFile = CreateFile(TEXT("../Bin/ToolSavedFiles/Navigation/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    for (int i = 0;i< (m_vNaviDatas.size()/3); ++i)
    {
        _float3		vPoints[3];
        XMStoreFloat3(&vPoints[0], XMLoadFloat3(&m_vNaviDatas[3*i]));
        XMStoreFloat3(&vPoints[1], XMLoadFloat3(&m_vNaviDatas[3*i+1]));
        XMStoreFloat3(&vPoints[2], XMLoadFloat3(&m_vNaviDatas[3*i+2]));
        
        WriteFile(hFile, &vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
    }

    CloseHandle(hFile);

    return S_OK;
}

void CImGui_Manager::ReturnNavi()
{
    if (0 >= m_vNaviDatas.size())
        return;

    /* 그리기 삭제 */
    m_pNavigationCom->DeleteCell();

    /* 저장용 vector 삭제 */
    vector<_float3>::iterator iter = m_vNaviDatas.end()-3;

    for(int i=0;i<3;++i)
        iter = m_vNaviDatas.erase(iter);
}

void CImGui_Manager::DrawPoints()
{
    //CTerrain* pTerrain = dynamic_cast<CTerrain*>(GET_LIST(LEVEL_STAGE1, L"Layer_BackGround")->front());
    //_float3* vPickPos = new _float3;
    //pTerrain->GetPickPos(vPickPos);
    _float3* vPickedPos = new _float3;
    ZeroMemory(vPickedPos, sizeof(_float3));

    if (KEY_PUSH(DIK_Z))
    {
        _float4* PickedPos = &m_pGameInstance->Get_PickedPos();

        vPickedPos->x = PickedPos->x;
        vPickedPos->y = PickedPos->y + 0.1f;
        vPickedPos->z = PickedPos->z;

        std::cout << " [m_pGameInstance.cpp] vPickPos : " << vPickedPos->x << ", " << vPickedPos->y << ", " << vPickedPos->z << endl;
        std::cout << "-----------------------------------------------------------" << endl;	// 월드좌표
    }

    if (vPickedPos->x == 0.f)
    {
        Safe_Delete(vPickedPos);
        return;
    }

    /* 점 좌표 보정작업 */
    Rounds(vPickedPos->x,1);
    Rounds(vPickedPos->y,1);
    Rounds(vPickedPos->z,1);
    
    m_fPoint[m_iPointCnt] = *vPickedPos;

    /* m_pNavigationCom 보여질 수 있도록 Cell 추가 */
    if (m_iPointCnt == 1)
    {
        m_pNavigationCom->AddCell(m_fPoint[0], m_fPoint[1]);
    }

    
    if (m_iPointCnt == 2)
    {
        /* 삼각형 시계방향 */
        SetPointsClockWise(m_fPoint);

        m_pNavigationCom->AddCell(m_fPoint[1], m_fPoint[2]);
        m_pNavigationCom->AddCell(m_fPoint[2], m_fPoint[0]);

        m_iPointCnt = 0;

        /* STL Vector 저장 */
        for (int i = 0; i < 3; ++i)
        {
            m_vNaviDatas.push_back(m_fPoint[i]);
            Safe_Delete(vPickedPos);
        }
        
        return;
    }
    else
    {
        m_iPointCnt++;
        Safe_Delete(vPickedPos);
    }
}

void CImGui_Manager::HideSavedNavi(_bool _b)
{
    CGameObject* pTerr = GET_LIST(LEVEL_STAGE1, L"Layer_BackGround")->front();
    dynamic_cast<CTerrain*>(pTerr)->NaviOnOff(_b);

}

string CImGui_Manager::ParsingJSON()
{
    json jDataList;
    for (auto& data : m_tOBJDatas)
    {
        json jData;
        jData["eObjType"] = data->eObjType;
        jData["fPosX"] = data->fPosX;
        jData["fPosY"] = data->fPosY;
        jData["fPosZ"] = data->fPosZ;
        jData["fRotY"] = data->fRotY;
        jData["sFileName"] = data->sFileName;
        jData["sLayerTag"] = data->sLayerTag;
        jData["sPrototpyeName"] = data->sPrototpyeName;
        jData["sComponentName"] = data->sComponentName;
        jDataList.push_back(jData);
    }

    return jDataList.dump();
}

HRESULT CImGui_Manager::SaveJSON(string _sJsonData, string _filePath)
{
    json jDataList = json::parse(_sJsonData);

    // JSON 파일에 쓰기
    std::ofstream ofs2(_filePath);
    if (!ofs2.is_open()) {
        std::cout << "Failed to open file for writing! " << std::endl;
    }

    ofs2 << std::setw(4) << jDataList << std::endl;
    ofs2.close();

    std::cout << "JSON 파일이 성공적으로 저장되었습니다." << std::endl;

    return S_OK;
}

HRESULT CImGui_Manager::LoadJSON()
{
    try {

        std::ifstream file("../Bin/ToolSavedFiles/test/data22.json", std::ios::in);
        if (!file.is_open())
        {
            std::cout << "Error opening data22.json.\n";
            return 1;
        }
        json j;
        file >> j;
        // JSON 값 출력
        std::cout << j["key1"] << std::endl;
        std::cout << j["key2"]["subkey"] << std::endl;

        return S_OK;
    }
    catch (std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return E_FAIL;
    }
}

void CImGui_Manager::Free()
{
    __super::Free();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    for (auto& iter : m_tOBJDatas)
        Safe_Delete(iter);
    m_tOBJDatas.clear();
    
    m_vNaviDatas.clear();

    Safe_Release(m_pNavigationCom);
}
