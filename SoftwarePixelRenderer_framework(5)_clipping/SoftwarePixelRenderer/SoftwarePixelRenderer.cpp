// SoftwarePixelRenderer.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "SoftwarePixelRenderer.h"
#include "CCube.h"
#include "CGameObject.h"
#include <functional>
#include <process.h>
#include <commdlg.h>
#include "CFpsCamera.h"
#include "CTpsCamera.h"
#include <random>

#define MAX_LOADSTRING 100

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

int SCREEN_BUFFER_WIDTH = 320;
int SCREEN_BUFFER_HEIGHT = 240;

const float SCREEN_DEPTH = 1000.0f;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HWND g_hWnd;
DWORD g_dwFPS = 0;

ID2D1SolidColorBrush* g_ipTextBrush = nullptr;
ID2D1SolidColorBrush* g_ipFpsBrush = nullptr;
ID2D1SolidColorBrush* g_ipWireBrush = nullptr;
ID2D1SolidColorBrush* g_ipLineBrush = nullptr;
ID2D1SolidColorBrush* g_ipRayBrush = nullptr;
ID2D1SolidColorBrush* g_ipPlayerBrush = nullptr;

IDWriteTextFormat* g_ipTextFormat = nullptr;

vector<CMesh*> g_vecMesh;

vector<CGameObject*>    g_vecObjects;

list<CGameObject*>  g_lstRender;

float g_fFov = XM_PI / 3.f;

int g_iObjectCount = 0;
int g_iFaceCount = 0;

int g_iRenderObjectCount = 0;
int g_iRenderFaceCount = 0;

const enum class CULL_BACKFACE
{
    NONE = 0,
    CCW = 1,
    CW = 2
};

CULL_BACKFACE g_CullBack = CULL_BACKFACE::NONE;

const enum class CAMERA_MODE : int
{
    FREE = 0,
    FPS = 1,
    TPS = 2
};

CAMERA_MODE g_CameraMode = CAMERA_MODE::FREE;

XMMATRIX g_matWorld;
XMMATRIX g_matView;
XMMATRIX g_matProj;
XMMATRIX g_matViewport;

XMMATRIX g_matRenderWorld;
XMMATRIX g_matRenderView;
XMMATRIX g_matRenderProj;
XMMATRIX g_matRenderViewport;

CRITICAL_SECTION g_csMouse;
CRITICAL_SECTION g_csRenderList;
CRITICAL_SECTION g_csRenderSync;

HANDLE g_hThreadProgress;
HANDLE g_hThreadRender;
HANDLE g_hThreadGeometry;

HANDLE g_hChangeResolution;

HANDLE g_hEventLoadResource;

bool g_bChangeResolution = false;
bool g_bRenderDone = true;
bool g_bPostRenderDone = true;
bool g_bWorkProgress = true;
bool g_bWorkRender = true;
bool g_bWorkGeometry = true;
bool g_bLoadResource = false;
bool g_bShowObject = true;
bool g_bShowBoundbox = false;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

unsigned WINAPI      ThreadProgress(LPVOID pArg);
unsigned WINAPI      ThreadRender(LPVOID pArg);
unsigned WINAPI      ThreadGeometry(LPVOID pArg);

BOOL InitializeSystem();
void ShutdownSystem();
void Progress();
void Render();
void RenderGeometry();


void ResetScreenResolution(HWND hWnd, const int cx, const int cy);
void ResizeScreenResolution(const int cx, const int cy);
void ResizeRenderingResolution(const int cx, const int cy);

void ResetResource();
void ShutdownResource();

void RenderObject(CGameObject* pObj, ID2D1RenderTarget* pRender, ID2D1SolidColorBrush* pBrush);
void DrawMesh(CMesh* pMesh, ID2D1RenderTarget* pRender, ID2D1SolidColorBrush* pBrush);

void ClearObject();
BOOL LoadObjFromFile(const wchar_t* szFileName);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SOFTWAREPIXELRENDERER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    if (false == InitializeSystem())
        return 0;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SOFTWAREPIXELRENDERER));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                break;
        }


    }

    ShutdownSystem();

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SOFTWAREPIXELRENDERER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SOFTWAREPIXELRENDERER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
   if (FAILED(hr)) return FALSE;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   HMENU hMenu = GetMenu(g_hWnd);
   CheckMenuItem(hMenu, ID_RESOLUTION_240P, MF_CHECKED);
   CheckMenuItem(hMenu, ID_BACKFACE_CULL_NONE, MF_CHECKED);

   CheckMenuItem(hMenu, ID_CAMERA_FREECAMERA, MF_CHECKED);

   CheckMenuItem(hMenu, ID_OBJECT_SHOW, MF_CHECKED);
   CheckMenuItem(hMenu, ID_BOUNDBOX_HIDE, MF_CHECKED);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case ID_RESOLUTION_120P:
            {
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_RESOLUTION_120P, MF_CHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_240P, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_480P, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_600P, MF_UNCHECKED);

                ResizeRenderingResolution(160, 120);
            }
            break;
            case ID_RESOLUTION_240P:
            {
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_RESOLUTION_120P, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_240P, MF_CHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_480P, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_600P, MF_UNCHECKED);

                ResizeRenderingResolution(320, 240);
            }
            break;
            case ID_RESOLUTION_480P:
            {
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_RESOLUTION_120P, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_240P, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_480P, MF_CHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_600P, MF_UNCHECKED);

                ResizeRenderingResolution(640, 480);
            }
            break;
            case ID_RESOLUTION_600P:
            {
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_RESOLUTION_120P, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_240P, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_480P, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_RESOLUTION_600P, MF_CHECKED);

                ResizeRenderingResolution(800, 600);
            }
            break;
            case ID_BACKFACE_CULL_NONE:
            {
                g_CullBack = CULL_BACKFACE::NONE;
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_BACKFACE_CULL_NONE, MF_CHECKED);
                CheckMenuItem(hMenu, ID_BACKFACE_CULL_CW, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_BACKFACE_CULL_CCW, MF_UNCHECKED);

            }
            break;
            case ID_BACKFACE_CULL_CCW:
            {
                g_CullBack = CULL_BACKFACE::CCW;
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_BACKFACE_CULL_NONE, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_BACKFACE_CULL_CW, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_BACKFACE_CULL_CCW, MF_CHECKED);
            }
            break;
            case ID_BACKFACE_CULL_CW:
            {
                g_CullBack = CULL_BACKFACE::CW;
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_BACKFACE_CULL_NONE, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_BACKFACE_CULL_CW, MF_CHECKED);
                CheckMenuItem(hMenu, ID_BACKFACE_CULL_CCW, MF_UNCHECKED);
            }
            break;
            case ID_CAMERA_FREECAMERA:
            {
                g_CameraMode = CAMERA_MODE::FREE;
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_CAMERA_FREECAMERA, MF_CHECKED);
                CheckMenuItem(hMenu, ID_CAMERA_FPSCAMERA, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_CAMERA_TPSCAMERA, MF_UNCHECKED);
            }
            break;
            case ID_CAMERA_FPSCAMERA:
            {
                g_CameraMode = CAMERA_MODE::FPS;
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_CAMERA_FREECAMERA, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_CAMERA_FPSCAMERA, MF_CHECKED);
                CheckMenuItem(hMenu, ID_CAMERA_TPSCAMERA, MF_UNCHECKED);
            }
            break;
            case ID_CAMERA_TPSCAMERA:
            {
                g_CameraMode = CAMERA_MODE::TPS;
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_CAMERA_FREECAMERA, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_CAMERA_FPSCAMERA, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_CAMERA_TPSCAMERA, MF_CHECKED);
            }
            break;
            case ID_IMPORT_OBJ:
            {
                wchar_t szFilePath[MAX_PATH] = { 0, };
                OPENFILENAME ofn;
                memset(&ofn, 0, sizeof(OPENFILENAME));
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = g_hWnd;
                ofn.lpstrFilter = L"Object(Wavefront) File(*.Obj)\0*.Obj\0All Files(*.*)\0*.*\0";
                ofn.lpstrFile = szFilePath;
                ofn.lpstrDefExt = L"Obj";
                ofn.nMaxFile = 256;
                
                if (GetOpenFileName(&ofn) != 0)
                {
                    g_bLoadResource = true;
                    LoadObjFromFile(szFilePath);
                    g_bLoadResource = false;
                }

            }
            break;
            case ID_DOC_NEW:
            {
                ClearObject();
            }
            break;
            case ID_OBJECT_SHOW:
            {
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_OBJECT_SHOW, MF_CHECKED);
                CheckMenuItem(hMenu, ID_OBJECT_HIDE, MF_UNCHECKED);
                g_bShowObject = true;
            }
                break;
            case ID_OBJECT_HIDE:
            {
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_OBJECT_SHOW, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_OBJECT_HIDE, MF_CHECKED);
                g_bShowObject = false;
            }
            break;
            case ID_BOUNDBOX_SHOW:
            {
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_BOUNDBOX_SHOW, MF_CHECKED);
                CheckMenuItem(hMenu, ID_BOUNDBOX_HIDE, MF_UNCHECKED);
                g_bShowBoundbox = true;
            }
            break;
            case ID_BOUNDBOX_HIDE:
            {
                HMENU hMenu = GetMenu(g_hWnd);
                CheckMenuItem(hMenu, ID_BOUNDBOX_SHOW, MF_UNCHECKED);
                CheckMenuItem(hMenu, ID_BOUNDBOX_HIDE, MF_CHECKED);
                g_bShowBoundbox = false;
            }
            break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_MOUSEMOVE:
        {
            static int xMove = 0, yMove = 0;
            int cx, cy;
            cx = LOWORD(lParam);
            cy = HIWORD(lParam);

            if (wParam == MK_MBUTTON)
            {
                SHORT shDeltaX, shDeltaY;

                shDeltaX = (SHORT)(cx - xMove);
                shDeltaY = (SHORT)(cy - yMove);


                CCamera* pCam = g_pCamera[(int)g_CameraMode];

                EnterCriticalSection(&g_csMouse);
                pCam->SetRotatePitchYawRollFactor(shDeltaX, shDeltaY, 0);
                LeaveCriticalSection(&g_csMouse);
            }

            xMove = cx;
            yMove = cy;
        }
        break;
        case WM_MOUSEWHEEL:
        {
            if (g_CameraMode == CAMERA_MODE::TPS)
            {
                int cx, cy;
                cx = LOWORD(lParam);
                cy = HIWORD(lParam);

                SHORT shDelta = (SHORT)HIWORD(wParam);

                CTpsCamera* pCam = dynamic_cast<CTpsCamera*>(g_pCamera[2]);

                EnterCriticalSection(&g_csMouse);
                if (shDelta < 0)
                    pCam->SetZoom(5.f);
                else
                    pCam->SetZoom(-5.f);
                LeaveCriticalSection(&g_csMouse);
            }

        }
        break;
    case WM_SIZE:
    {
        int cx, cy;
        cx = LOWORD(lParam);
        cy = HIWORD(lParam);

        if (g_pD2dSystem)
            ResizeScreenResolution(cx, cy);
    }
    break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


BOOL InitializeSystem()
{
    InitializeCriticalSection(&g_csMouse);
    InitializeCriticalSection(&g_csRenderList);
    InitializeCriticalSection(&g_csRenderSync);


    g_pD2dSystem = new CD2dSystem();
    if (false == g_pD2dSystem->Initialize(g_hWnd, 800, 600))
        return FALSE;

    ResetResource();

    std::mt19937_64 rng(2233);
    std::uniform_real_distribution<float> dist(-30, 30);


    CMesh* pMesh = new CCube;
    pMesh->Initialize();
    g_vecMesh.push_back(pMesh);

    g_vecObjects.resize(100);
    for (int i = 0; i < 100; ++i)
    {
        g_vecObjects[i] = new CGameObject;
        g_vecObjects[i]->SetPosition(XMFLOAT3(dist(rng), dist(rng), dist(rng)));
        g_vecObjects[i]->AttachModel(pMesh);
    }

    g_pCamera[0] = new CCamera;
    g_pCamera[0]->SetPosition(XMFLOAT3(0, 0, -5));

    g_pCamera[1] = new CFpsCamera;
    g_pCamera[1]->SetPosition(XMFLOAT3(2, 0, -5));

    g_pCamera[2] = new CTpsCamera;
    g_pCamera[2]->SetPosition(XMFLOAT3(0, 0, -1));
    ((CTpsCamera*)g_pCamera[2])->SetZoom(4);

    //Worker thread

    g_hChangeResolution = CreateEvent(0, 0, 0, 0);
    g_hEventLoadResource = CreateEvent(0, 0, 0, 0);


    g_bWorkProgress = true;
    g_bWorkRender = true;
    g_bWorkGeometry = true;

    g_hThreadProgress = (HANDLE)_beginthreadex(0, 0, ThreadProgress, 0, 0, 0);
    g_hThreadRender = (HANDLE)_beginthreadex(0, 0, ThreadRender, 0, 0, 0);
    g_hThreadGeometry = (HANDLE)_beginthreadex(0, 0, ThreadGeometry, 0, 0, 0);

    return TRUE;
}

void ShutdownSystem()
{
    g_bWorkProgress = false;
    g_bWorkRender = false;
    g_bWorkGeometry = false;

    SetEvent(g_hEventLoadResource);

    Sleep(1000);

    DWORD dwExitCode;

    if (TRUE == GetExitCodeThread(g_hThreadProgress, &dwExitCode))
    {
        if (dwExitCode == STILL_ACTIVE)
            TerminateThread(g_hThreadProgress, 0);
    }

    if (TRUE == GetExitCodeThread(g_hThreadRender, &dwExitCode))
    {
        if (dwExitCode == STILL_ACTIVE)
            TerminateThread(g_hThreadRender, 0);
    }
    if (TRUE == GetExitCodeThread(g_hThreadGeometry, &dwExitCode))
    {
        if (dwExitCode == STILL_ACTIVE)
            TerminateThread(g_hThreadGeometry, 0);
    }

    CloseHandle(g_hThreadProgress);
    g_hThreadProgress = NULL;
    CloseHandle(g_hThreadRender);
    g_hThreadRender = NULL;
    CloseHandle(g_hThreadGeometry);
    g_hThreadGeometry = NULL;
    CloseHandle(g_hEventLoadResource);
    g_hEventLoadResource = NULL;
    CloseHandle(g_hChangeResolution);
    g_hChangeResolution = NULL;
    
    ShutdownResource();

    for (size_t i = 0; i < g_vecObjects.size(); ++i)
    {
        delete g_vecObjects[i];
        g_vecObjects[i] = nullptr;
    }
    g_vecObjects.clear();

    for (size_t i = 0; i < g_vecMesh.size(); ++i)
    {
        g_vecMesh[i]->Shutdown();
        delete g_vecMesh[i];
        g_vecMesh[i] = nullptr;
    }
    g_vecMesh.clear();

    for (int i = 0; i < 3; ++i)
    {
        if (g_pCamera[i])
        {
            delete g_pCamera[i];
            g_pCamera[i] = nullptr;
        }
    }
    
    if (g_pD2dSystem)
    {
        g_pD2dSystem->Shutdown();
        delete g_pD2dSystem;
        g_pD2dSystem = nullptr;
    }

    CoUninitialize();

    DeleteCriticalSection(&g_csMouse);
    DeleteCriticalSection(&g_csRenderList);
    DeleteCriticalSection(&g_csRenderSync);
}

void Progress()
{
    XMMATRIX matWorld, matView, matProj, matViewport;
    matProj = XMMatrixPerspectiveFovLH(g_fFov, (float)SCREEN_BUFFER_WIDTH / SCREEN_BUFFER_HEIGHT, 1, SCREEN_DEPTH);

    static DWORD dwOldTick = 0;
    DWORD dwTick = GetTickCount();
    if (dwOldTick == 0) dwOldTick = dwTick;
    float fEllipse = (dwTick - dwOldTick) / SCREEN_DEPTH;

    dwOldTick = dwTick;
    
    //뷰 행렬 만들기
    XMFLOAT4X4 matCamera;

    g_pCamera[(int)g_CameraMode]->Render(fEllipse);
    g_pCamera[(int)g_CameraMode]->MakeMatrix();

    EnterCriticalSection(&g_csMouse);
    g_pCamera[(int)g_CameraMode]->MakeMatrix();
    LeaveCriticalSection(&g_csMouse);

    matCamera = g_pCamera[(int)g_CameraMode]->GetMatrix();

    matView = XMLoadFloat4x4(&matCamera);


    //Matrix transform : scaling -> local rotate -> move -> revolution -> parent
    //world x view x projection x viewport matrix 순서
    matWorld = XMMatrixIdentity();//XMMatrixRotationZ(0.3f);
    matViewport = XMMatrixViewport(SCREEN_BUFFER_WIDTH, SCREEN_BUFFER_HEIGHT);

    int iObjectCount = 0;

    //frustum boundbox culling
    list<CGameObject*> lstCull;
    CGameObject* pObj;

    XMVECTOR vCube[8];
    VERTEX* pCubeVertices = nullptr;

    XMFLOAT3 v3Pos;
    XMMATRIX matTranslation, matTransform, matModel;
    CFrustumClass frustum;

    for (size_t i = 0; i < g_vecObjects.size(); ++i)
    {
        pObj = g_vecObjects[i];
        pObj->GetPosition(&v3Pos);
        matTranslation = XMMatrixTranslation(v3Pos.x, v3Pos.y, v3Pos.z);
        matTransform = pObj->GetTransform();
        matModel = XMMatrixMultiply(matTranslation, matTransform);

        XMMATRIX matFrustumView;
        XMMATRIX matMW = XMMatrixMultiply(matModel, matWorld);
        matFrustumView = XMMatrixMultiply(matMW, matView);

        frustum.ConstructFrustum(SCREEN_DEPTH, matProj, matFrustumView);

        pCubeVertices = g_vecObjects[i]->GetBoundbox()->GetModel()->vecVertices.data();

        for (size_t j = 0; j < 8; ++j)
        {
            XMVECTOR vTrans = XMLoadFloat3(&pCubeVertices[j].v3Vtx);
            vCube[j] = vTrans;
        }

        //insert to render queue
        if (true == frustum.CheckCube(vCube))
        {
            lstCull.push_back(g_vecObjects[i]);
            iObjectCount++;
        }

    }

    EnterCriticalSection(&g_csRenderList);
    g_matWorld = matWorld;
    g_matView = matView;
    g_matProj = matProj;
    g_matViewport = matViewport;
    g_iObjectCount = iObjectCount;
    g_lstRender.clear();
    g_lstRender.insert(g_lstRender.end(), lstCull.begin(), lstCull.end());

    //더블버퍼링을 위한 클리어 화면 갱신
    if (lstCull.size() == 0)
    {
        g_pD2dSystem->ClearScreen(D2D1::ColorF(RGB(122, 10, 10)));
    }

    LeaveCriticalSection(&g_csRenderList);

    
}

void Render()
{
    g_bPostRenderDone = false;

    CRect rc;
    ::GetClientRect(g_hWnd, &rc);

    DWORD dwFps = 0;
    int iFace = 0;
    int iObjCnt = 0;

    ID2D1Bitmap* pBack = nullptr;

    EnterCriticalSection(&g_csRenderSync);
    dwFps = g_dwFPS;
    iObjCnt = g_iRenderObjectCount;
    iFace = g_iRenderFaceCount;
    pBack = CWicBitmap::ConvertWicToD2dBitmap(dfRENDERTARGET(), g_pD2dSystem->GetBackbuffer());
    LeaveCriticalSection(&g_csRenderSync);

    dfRENDERTARGET()->BeginDraw();

    if (pBack)
    {
        D2D1_RECT_F rcScreen, rcSrc;
        rcScreen.left = 0;
        rcScreen.top = 0;
        rcScreen.right = (float)rc.Width();
        rcScreen.bottom = (float)rc.Height();
        rcSrc.left = 0;
        rcSrc.top = 0;
        rcSrc.right = (float)SCREEN_BUFFER_WIDTH;
        rcSrc.bottom = (float)SCREEN_BUFFER_HEIGHT;

        dfRENDERTARGET()->DrawBitmap(pBack, rcScreen, 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, rcSrc);
    }



    WCHAR szFPS[40] = { 0, };
    wsprintfW(szFPS, L"FPS : %d", dwFps);

    D2D1_RECT_F rcFPS = D2D1::RectF(5, 5, 90, 10);
    dfRENDERTARGET()->DrawText(szFPS, (uint32_t)wcslen(szFPS), g_ipTextFormat, rcFPS, g_ipFpsBrush);

    wsprintfW(szFPS, L"Total Object : %d", iObjCnt);
    rcFPS = D2D1::RectF(5, 20, 120, 25);
    dfRENDERTARGET()->DrawText(szFPS, (uint32_t)wcslen(szFPS), g_ipTextFormat, rcFPS, g_ipFpsBrush);

    wsprintfW(szFPS, L"Total face : %d", iFace);
    rcFPS = D2D1::RectF(5, 35, 120, 40);
    dfRENDERTARGET()->DrawText(szFPS, (uint32_t)wcslen(szFPS), g_ipTextFormat, rcFPS, g_ipFpsBrush);

    //렌더링 끝
    if (D2DERR_RECREATE_TARGET == dfRENDERTARGET()->EndDraw())
    {
        ResetScreenResolution(g_hWnd, rc.Width(), rc.Height());
    }

    gf_SafeRelease(pBack);

    g_bPostRenderDone = true;

}

void RenderGeometry()
{
    CRect rc;
    ::GetClientRect(g_hWnd, &rc);
    g_bRenderDone = false;

    int iObjCount = 0;

    static DWORD dwFPS = 0;
    static DWORD dwFPSCount = 0;
    static DWORD dwTickOld = 0;

    list<CGameObject*> lstRenderObj;

    EnterCriticalSection(&g_csRenderList);
    g_matRenderWorld = g_matWorld;
    g_matRenderView = g_matView;
    g_matRenderProj = g_matProj;
    g_matRenderViewport = g_matViewport;
    iObjCount = g_iObjectCount;
    lstRenderObj.insert(lstRenderObj.end(), g_lstRender.begin(), g_lstRender.end());
    g_lstRender.clear();

    LeaveCriticalSection(&g_csRenderList);

    g_iFaceCount = 0;

    DWORD dwTick = GetTickCount();

    if (dwTickOld == 0)
        dwTickOld = dwTick;

    if (lstRenderObj.size() != 0)
    {
        dfBMPRENDERTARGET()->BeginDraw();
        dfBMPRENDERTARGET()->Clear(D2D1::ColorF(RGB(122, 10, 10)));

        list<CGameObject*>::iterator iter;
        for (iter = lstRenderObj.begin(); iter != lstRenderObj.end(); ++iter)
            RenderObject(*iter, dfBMPRENDERTARGET(), g_ipWireBrush);


        if (D2DERR_RECREATE_TARGET == dfBMPRENDERTARGET()->EndDraw())
        {
            ResetScreenResolution(g_hWnd, rc.Width(), rc.Height());
        }

    }

     //더블 버퍼링
     EnterCriticalSection(&g_csRenderSync);
     g_dwFPS = dwFPS;
     g_iRenderObjectCount = iObjCount;
     g_iRenderFaceCount = g_iFaceCount;
     if (lstRenderObj.size())
	g_pD2dSystem->SwapBuffer();
     LeaveCriticalSection(&g_csRenderSync);



    if (dwTick - dwTickOld > 1000)
    {
        dwTickOld = dwTick;
        dwFPS = dwFPSCount;
        dwFPSCount = 0;
    }

    dwFPSCount++;

    g_bRenderDone = true;
}




void ResetScreenResolution(HWND hWnd, const int cx, const int cy)
{
    if (g_pD2dSystem != nullptr)
    {
        g_pD2dSystem->ResetRenderTarget(hWnd, cx, cy);

        ResetResource();
    }
}

void ResizeScreenResolution(const int cx, const int cy)
{
    g_bChangeResolution = true;

    WaitForSingleObject(g_hChangeResolution, INFINITE);
    while (false == g_bRenderDone || false == g_bPostRenderDone)
        Sleep(10);

    if (g_pD2dSystem != nullptr)
    {
        g_pD2dSystem->ResizeRenderTarget(cx, cy);
    }



    g_bChangeResolution = false;
}

void ResizeRenderingResolution(const int cx, const int cy)
{
    g_bChangeResolution = true;

    WaitForSingleObject(g_hChangeResolution, INFINITE);
    while (false == g_bRenderDone || false == g_bPostRenderDone)
        Sleep(10);

    //if (g_pD2dSystem != nullptr)
    //{
    //    g_pD2dSystem->ResizeBuffer(cx, cy);
    //}

    EnterCriticalSection(&g_csRenderList);
    SCREEN_BUFFER_WIDTH = cx;
    SCREEN_BUFFER_HEIGHT = cy;
    LeaveCriticalSection(&g_csRenderList);

    g_bChangeResolution = false;

}

void ShutdownResource()
{
    gf_SafeRelease(g_ipTextBrush);
    gf_SafeRelease(g_ipFpsBrush);
    gf_SafeRelease(g_ipTextFormat);
    gf_SafeRelease(g_ipWireBrush);
    gf_SafeRelease(g_ipLineBrush);
    gf_SafeRelease(g_ipRayBrush);
    gf_SafeRelease(g_ipPlayerBrush);
}


void RenderObject(CGameObject* pObj, ID2D1RenderTarget* pRender, ID2D1SolidColorBrush* pBrush)
{
    pObj->Render();
    
    CMesh* pModel = pObj->GetModel();
    if (pModel && g_bShowObject)
        DrawMesh(pModel, pRender, pBrush);

    if(g_bShowBoundbox)
        DrawMesh(pObj->GetBoundbox(), pRender, pBrush);
}


void DrawMesh(CMesh* pMesh, ID2D1RenderTarget* pRender, ID2D1SolidColorBrush* pBrush)
{
    typedef struct _tagEdge
    {
        D2D1_POINT_2F ptStart;
        D2D1_POINT_2F ptEnd;

        bool operator == (const _tagEdge& edge)
        {
            if (ptStart == edge.ptStart && ptEnd == edge.ptEnd ||
                ptStart == edge.ptEnd && ptEnd == edge.ptStart)
                return true;

            return false;
        }
    }EDGE;

    list<EDGE> lstCheckOverlapLine;
    function<bool(list<EDGE>& lst, EDGE&)> fcCheckOverlap = [&](list<EDGE>& lst, EDGE& line)->bool
    {
        list<EDGE>::iterator iter;

        for (iter = lst.begin(); iter != lst.end(); ++iter)
        {
            if (*iter == line)
                return true;
        }

        lst.push_back(line);

        return false;
    };

    XMFLOAT3 v3Conv[4], v3ConvWorld[4];
    XMVECTOR vCalc;

    /*
                 -1 ━━━━━━0━━━━━━  1   plane
                   ↖          ↑         ↗
                      ↖       ↑dir   ↗
                         ↖    ↑   ↗
                            ↖ ↑↗
                               pos

        투영 공간에서는 시야가 평면과 항상 90도로 바라보기 때문에
        수직 관계이면 cos(90)도 값이 0 이다.
        +-cos(180)도가 넘어가면 바라보는 시야가 평면 뒤에 있다는 의미이다.

    */

    XMFLOAT3 v3Look = XMFLOAT3(0, 0, 1);
    XMVECTOR vLook = XMLoadFloat3(&v3Look);

    //Converts  camera direction frorm face to camera direction.
    //vLook *= -1;

    XMMATRIX matModel = pMesh->GetModelMatrix();
    XMMATRIX matMW = XMMatrixMultiply(matModel, g_matRenderWorld);
    XMMATRIX matFrustumView = XMMatrixMultiply(matMW, g_matRenderView);
    XMMATRIX matVP = XMMatrixMultiply(g_matRenderView, g_matRenderProj);
    XMMATRIX matMWVPV = XMMatrixMultiply(matMW, matVP);
    matMWVPV = XMMatrixMultiply(matMWVPV, g_matRenderViewport);

    CFrustumClass frustum;
    frustum.ConstructFrustum(SCREEN_DEPTH, g_matRenderProj, matFrustumView);

    int iFaceInVertexCount = 0;

    MESH_MODEL* pModels = pMesh->GetModel();
    size_t tModelCount = pMesh->GetModelCount();

    CFace* pFacies = nullptr;
    size_t tFaceCount = 0;

    for (size_t model_cnt = 0; model_cnt < tModelCount; ++model_cnt)
    {
        tFaceCount = pModels[model_cnt].vecFacies.size();
        pFacies = pModels[model_cnt].vecFacies.data();

        list<CFace> lstClipFacies;
        list<CFace>::iterator iter;
        CFace cullFace, clipA, clipB;

        for (size_t cn = 0; cn < tFaceCount; cn++)
        {
            cullFace = pFacies[cn];

            iFaceInVertexCount = (int)cullFace.GetVertexCount();

            VERTEX* pFaceVertices = cullFace.GetVertices();

            if (iFaceInVertexCount == 3)
                clipA.SetFace(pFaceVertices, 3);
            else
            {
                clipA.SetFace(pFaceVertices[0], pFaceVertices[1], pFaceVertices[3]);
                clipB.SetFace(pFaceVertices[1], pFaceVertices[2], pFaceVertices[3]);
            }

            //vertex view translation
            for (int i = 0; i < iFaceInVertexCount; ++i)
            {
                //face in world space
                vCalc = XMLoadFloat3(&pFaceVertices[i].v3Vtx);
                vCalc = XMVector3TransformCoord(vCalc, matMW);
                XMStoreFloat3(&v3ConvWorld[i], vCalc);
                //face in view x proj space
                vCalc = XMVector3TransformCoord(vCalc, matVP);
                XMStoreFloat3(&v3Conv[i], vCalc);

                //Transform face for backface culling
                pFaceVertices[i].v3Vtx = v3Conv[i];
            }

            cullFace.MakeNormal();
            float fCosTheta = cullFace.CalcVectorDotCosf(vLook);

            //Back face culling
            //Must be calculated in projection space.
            if (g_CullBack == CULL_BACKFACE::CW)
            {
                if (XM_PIDIV2 > fCosTheta)
                {
                    continue;
                }
            }
            else if (g_CullBack == CULL_BACKFACE::CCW)
            {
                if (XM_PIDIV2 <= fCosTheta)
                {
                    continue;
                }
            }


            lstClipFacies.clear();

            if (iFaceInVertexCount == 3)
                frustum.FaceClipAgainstPlane(clipA, lstClipFacies);
            else
            {
                frustum.FaceClipAgainstPlane(clipA, lstClipFacies);
                frustum.FaceClipAgainstPlane(clipB, lstClipFacies);
            }

            if (lstClipFacies.size())
            {
                list<CFace>::iterator iter;
                size_t tVtxCnt = 0;
                for (iter = lstClipFacies.begin(); iter != lstClipFacies.end(); ++iter)
                {
                    VERTEX* pVertices = iter->GetVertices();
                    tVtxCnt = iter->GetVertexCount();
                    for (int i = 0; i < tVtxCnt; ++i)
                    {
                        vCalc = XMLoadFloat3(&pVertices[i].v3Vtx);
                        vCalc = XMVector3TransformCoord(vCalc, matMWVPV);

                        XMStoreFloat3(&v3Conv[i], vCalc);
                    }

                    for (int i = 0; i < tVtxCnt; ++i)
                    {
                        EDGE edge;
                        edge.ptStart.x = v3Conv[i % tVtxCnt].x;
                        edge.ptStart.y = v3Conv[i % tVtxCnt].y;

                        edge.ptEnd.x = v3Conv[(i + 1) % tVtxCnt].x;
                        edge.ptEnd.y = v3Conv[(i + 1) % tVtxCnt].y;

                        fcCheckOverlap(lstCheckOverlapLine, edge);
                    }

                    g_iFaceCount++;
                }

            }
        }

    }

    list<EDGE>::iterator iter;
    for (iter = lstCheckOverlapLine.begin(); iter != lstCheckOverlapLine.end(); ++iter)
    {
        pRender->DrawLine(iter->ptStart, iter->ptEnd, pBrush);

    }

}


void ClearObject()
{
    g_bLoadResource = true;
    WaitForSingleObject(g_hEventLoadResource, INFINITE);

    while (false == g_bRenderDone)
        Sleep(1);

    for (size_t i = 0; i < g_vecMesh.size(); ++i)
    {
        g_vecMesh[i]->Shutdown();
        delete g_vecMesh[i];
        g_vecMesh[i] = nullptr;
    }
    g_vecMesh.clear();

    for (size_t i = 0; i < g_vecObjects.size(); ++i)
    {
        delete g_vecObjects[i];
        g_vecObjects[i] = nullptr;
    }
    g_vecObjects.clear();

    g_lstRender.clear();

    //스크린 버퍼 클리어
    g_pD2dSystem->ClearScreen(D2D1::ColorF(RGB(122, 10, 10)));

    g_bLoadResource = false;
}

BOOL LoadObjFromFile(const wchar_t* szFileName)
{
    BOOL bRes = FALSE;
    WaitForSingleObject(g_hEventLoadResource, INFINITE);

    while (false == g_bRenderDone)
        Sleep(1);

    CMesh* pMesh = new CMesh();
    
    bRes = pMesh->LoadFromFile(szFileName);


    if (FALSE == bRes)
    {
        pMesh->Shutdown();
        delete pMesh;
        return FALSE;
    }
    else
        g_vecMesh.push_back(pMesh);

    CGameObject* pObj = new CGameObject;
    pObj->AttachModel(pMesh);
    g_vecObjects.push_back(pObj);
    

    return TRUE;
}



void ResetResource()
{
    ShutdownResource();

    HRESULT hr;
    static const WCHAR msc_fontName[] = L"Verdana";
    static FLOAT msc_fontSize = 10;


    if (FAILED(dfRENDERTARGET()->CreateSolidColorBrush(D2D1::ColorF(RGB(0, 201, 175), 1), &g_ipFpsBrush)))
    {
        printf("Failed to create Fps's solid brush...");
        return;
    }

    if (FAILED(dfBMPRENDERTARGET()->CreateSolidColorBrush(D2D1::ColorF(RGB(0, 201, 175), 1), &g_ipTextBrush)))
    {
        printf("Failed to create Text's solid brush...");
        return;
    }

    if (FAILED(dfBMPRENDERTARGET()->CreateSolidColorBrush(D2D1::ColorF(RGB(255, 255, 255), 1), &g_ipWireBrush)))
    {
        printf("Failed to carete Wall's solid brush...");
        return;
    }

    if (FAILED(dfBMPRENDERTARGET()->CreateSolidColorBrush(D2D1::ColorF(RGB(0, 255, 0), 1), &g_ipLineBrush)))
    {
        printf("Failed to carete Edge line's solid brush...");
        return;
    }

    if (FAILED(dfBMPRENDERTARGET()->CreateSolidColorBrush(D2D1::ColorF(RGB(255, 255, 255), 1), &g_ipRayBrush)))
    {
        printf("Failed to carete Ray's solid brush...");
        return;
    }

    if (FAILED(dfBMPRENDERTARGET()->CreateSolidColorBrush(D2D1::ColorF(RGB(255, 100, 20), 1), &g_ipPlayerBrush)))
    {
        printf("Failed to carete Player's solid brush...");
        return;
    }


    hr = dfWRITEFACTORY()->CreateTextFormat(msc_fontName, NULL, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        msc_fontSize,
        L"",
        &g_ipTextFormat);

    if (FAILED(hr))
    {
        printf("Failed to create TextFormat...");
        return;
    }
    g_ipTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    g_ipTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}




unsigned __stdcall ThreadProgress(LPVOID pArg)
{
    while (1)
    {
        if (g_bWorkProgress == false) break;

        if (g_bChangeResolution == true)
        {
            Sleep(30);
            continue;
        }

        if (g_bLoadResource == true)
        {
            Sleep(30);
            continue;
        }

        Progress();
    }

    return 0;
}

unsigned __stdcall ThreadRender(LPVOID pArg)
{
    while (1)
    {
        if (g_bWorkRender == false) break;

        if (g_bChangeResolution == true)
        {

            Sleep(30);
            continue;
        }

        if (g_bLoadResource == true)
        {

            Sleep(30);
            continue;
        }

        Render();

    }

    return 0;
}

unsigned __stdcall ThreadGeometry(LPVOID pArg)
{
    while (1)
    {
        if (g_bWorkGeometry == false) break;

        if (g_bChangeResolution == true)
        {
            SetEvent(g_hChangeResolution);
            Sleep(30);
            continue;
        }

        if (g_bLoadResource == true)
        {
            SetEvent(g_hEventLoadResource);
            Sleep(30);
            continue;
        }

        RenderGeometry();

    }

    return 0;
}