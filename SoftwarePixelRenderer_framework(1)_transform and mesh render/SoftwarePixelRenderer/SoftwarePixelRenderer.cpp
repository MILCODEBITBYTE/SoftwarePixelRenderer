// SoftwarePixelRenderer.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "SoftwarePixelRenderer.h"
#include <process.h>
#include "CCube.h"

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

ID2D1SolidColorBrush* g_ipTextBrush = nullptr;
ID2D1SolidColorBrush* g_ipFpsBrush = nullptr;
ID2D1SolidColorBrush* g_ipWireBrush = nullptr;
ID2D1SolidColorBrush* g_ipLineBrush = nullptr;
ID2D1SolidColorBrush* g_ipRayBrush = nullptr;
ID2D1SolidColorBrush* g_ipPlayerBrush = nullptr;

IDWriteTextFormat* g_ipTextFormat = nullptr;

CWicBitmap* g_pTex = nullptr;

CMesh* g_pCube = nullptr;

float g_fFov = XM_PIDIV2;

const enum class CULL_BACKFACE
{
    NONE = 0,
    CCW = 1,
    CW = 2
};

CULL_BACKFACE g_CullBack = CULL_BACKFACE::NONE;

XMMATRIX g_matWorld;
XMMATRIX g_matProj;
XMMATRIX g_matViewport;

XMMATRIX g_matRenderWorld;
XMMATRIX g_matRenderProj;
XMMATRIX g_matRenderViewport;

CRITICAL_SECTION g_csRenderList;

HANDLE g_hChangeResolution;

HANDLE g_hThreadProgress;
HANDLE g_hThreadRender;

HANDLE g_hEventLoadResource;

bool g_bChangeResolution = false;
bool g_bRenderDone = true;
bool g_bWorkProgress = true;
bool g_bWorkRender = true;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

unsigned WINAPI      ThreadProgress(LPVOID pArg);
unsigned WINAPI      ThreadRender(LPVOID pArg);

BOOL InitializeSystem();
void ShutdownSystem();
void Progress();
void Render();

void ResetScreenResolution(HWND hWnd, const int cx, const int cy);
void ResizeScreenResolution(const int cx, const int cy);
void ResizeRenderingResolution(const int cx, const int cy);

void ResetResource();
void ShutdownResource();

void DrawMesh(CMesh* pMesh, ID2D1RenderTarget* pRender, ID2D1SolidColorBrush* pBrush);

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
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
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
    InitializeCriticalSection(&g_csRenderList);


    g_pD2dSystem = new CD2dSystem();
    if (false == g_pD2dSystem->Initialize(g_hWnd, 800, 600))
        return FALSE;

    ResetResource();


    g_pTex = new CWicBitmap;
    g_pTex->LoadWicBitmap(dfRENDERTARGET(), L"./Resources/wall.jpg");

    g_pCube = new CCube;
    g_pCube->Initialize();

    //Worker thread

    g_hChangeResolution = CreateEvent(0, 0, 0, 0);

    g_bWorkProgress = true;
    g_bWorkRender = true;

    g_hThreadProgress = (HANDLE)_beginthreadex(0, 0, ThreadProgress, 0, 0, 0);
    g_hThreadRender = (HANDLE)_beginthreadex(0, 0, ThreadRender, 0, 0, 0);

    return TRUE;
}

void ShutdownSystem()
{
    g_bWorkProgress = false;
    g_bWorkRender = false;


    Sleep(10);

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

    CloseHandle(g_hThreadProgress);
    g_hThreadProgress = NULL;
    CloseHandle(g_hThreadRender);
    g_hThreadRender = NULL;
    CloseHandle(g_hChangeResolution);
    g_hChangeResolution = NULL;

    ShutdownResource();

    if (g_pTex)
    {
        g_pTex->ShutdownResource();
        delete g_pTex;
        g_pTex = nullptr;
    }

    if (g_pCube)
    {
        delete g_pCube;
        g_pCube = nullptr;
    }

    if (g_pD2dSystem)
    {
        g_pD2dSystem->Shutdown();
        delete g_pD2dSystem;
        g_pD2dSystem = nullptr;
    }

    CoUninitialize();

    DeleteCriticalSection(&g_csRenderList);
}

void Progress()
{
    XMMATRIX matWorld, matProj, matViewport;
    matProj = XMMatrixPerspectiveFovLH(g_fFov, (float)SCREEN_BUFFER_WIDTH / SCREEN_BUFFER_HEIGHT, 1, SCREEN_DEPTH);

    static DWORD dwOldTick = 0;
    DWORD dwTick = GetTickCount();
    if (dwOldTick == 0) dwOldTick = dwTick;
    float fEllipse = (dwTick - dwOldTick) / SCREEN_DEPTH;

    dwOldTick = dwTick;

    static float fRot = 0;
    if (fRot > XM_2PI) fRot = 0;


    //Matrix transform : scaling -> local rotate -> move -> revolution -> parent


    matWorld = XMMatrixRotationRollPitchYaw(0, fRot, 0);
    matWorld = XMMatrixMultiply(matWorld, XMMatrixTranslation(0, -0.25f, 3.25f));

    matViewport = XMMatrixViewport(SCREEN_BUFFER_WIDTH, SCREEN_BUFFER_HEIGHT);

    fRot += 3 * fEllipse;

    EnterCriticalSection(&g_csRenderList);
    g_matWorld = matWorld;
    g_matProj = matProj;
    g_matViewport = matViewport;
    LeaveCriticalSection(&g_csRenderList);
}

void Render()
{
    g_bRenderDone = false;

    EnterCriticalSection(&g_csRenderList);
    g_matRenderWorld = g_matWorld;
    g_matRenderProj = g_matProj;
    g_matRenderViewport = g_matViewport;
    LeaveCriticalSection(&g_csRenderList);

    static DWORD dwFPS = 0;
    static DWORD dwFPSCount = 0;
    static DWORD dwTickOld = 0;

    DWORD dwTick = GetTickCount();

    if (dwTickOld == 0)
        dwTickOld = dwTick;

    CRect rc;
    ::GetClientRect(g_hWnd, &rc);

    dfBMPRENDERTARGET()->BeginDraw();
    dfBMPRENDERTARGET()->Clear(D2D1::ColorF(RGB(122, 10, 10)));

    DrawMesh(g_pCube, dfBMPRENDERTARGET(), g_ipWireBrush);



    if (D2DERR_RECREATE_TARGET == dfBMPRENDERTARGET()->EndDraw())
    {
        ResetScreenResolution(g_hWnd, rc.Width(), rc.Height());
    }


    ID2D1Bitmap* pBack = CWicBitmap::ConvertWicToD2dBitmap(dfRENDERTARGET(), g_pD2dSystem->GetRenderbuffer());


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
    wsprintfW(szFPS, L"FPS : %d", dwFPS);

    D2D1_RECT_F rcFPS = D2D1::RectF(5, 5, 60, 20);

    dfRENDERTARGET()->DrawText(szFPS, (uint32_t)wcslen(szFPS), g_ipTextFormat, rcFPS, g_ipFpsBrush);

    //렌더링 끝
    if (D2DERR_RECREATE_TARGET == dfRENDERTARGET()->EndDraw())
    {
        ResetScreenResolution(g_hWnd, rc.Width(), rc.Height());;
    }

    gf_SafeRelease(pBack);

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
    while (false == g_bRenderDone)
        Sleep(1);

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
    while (false == g_bRenderDone)
        Sleep(1);

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


void DrawMesh(CMesh* pMesh, ID2D1RenderTarget* pRender, ID2D1SolidColorBrush* pBrush)
{

    XMMATRIX matWP, matWPV;

    matWP = XMMatrixMultiply(g_matRenderWorld, g_matRenderProj);
    matWPV = XMMatrixMultiply(matWP, g_matRenderViewport);

    size_t tVertexCount = pMesh->GetVertexCount();
    size_t tIndexCount = pMesh->GetIndexCount();

    XMFLOAT3* pVertices = pMesh->GetVertices();
    DWORD* pIndecies = pMesh->GetIndices();

    XMFLOAT3 v3Result[3], v3Conv[3];
    XMVECTOR vCalc[3];

    D2D1_POINT_2F ptLine[2];

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


    XMFLOAT3 v3Look(0, 0, 1);
    XMVECTOR vLook = XMLoadFloat3(&v3Look);

    //Converts  camera direction frorm face to camera direction.
    //vLook *= -1;

    for (size_t cn = 0; cn < tIndexCount; cn+=3)
    {
        
        vCalc[0] = XMLoadFloat3(&pVertices[pIndecies[cn + 0]]);
        vCalc[1] = XMLoadFloat3(&pVertices[pIndecies[cn + 1]]);
        vCalc[2] = XMLoadFloat3(&pVertices[pIndecies[cn + 2]]);


        //Back face culling
        //Must be calculated in view or projection space.
        XMStoreFloat3(&v3Conv[0], XMVector3TransformCoord(vCalc[0], matWP));
        XMStoreFloat3(&v3Conv[1], XMVector3TransformCoord(vCalc[1], matWP));
        XMStoreFloat3(&v3Conv[2], XMVector3TransformCoord(vCalc[2], matWP));

        CFace face(v3Conv[0], v3Conv[1], v3Conv[2]);
        face.MakeNormal();
        float fCosTheta = face.CalcVectorDotCosf(vLook);

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

        //vertex world x projection x viewport translation
        XMStoreFloat3(&v3Result[0], XMVector3TransformCoord(vCalc[0], matWPV));
        XMStoreFloat3(&v3Result[1], XMVector3TransformCoord(vCalc[1], matWPV));
        XMStoreFloat3(&v3Result[2], XMVector3TransformCoord(vCalc[2], matWPV));
        
        //viewport
        for (int i = 0; i < 3; ++i)
        {
            ptLine[0].x = v3Result[i % 3].x;
            ptLine[0].y = v3Result[i % 3].y;

            ptLine[1].x = v3Result[(i + 1) % 3].x;
            ptLine[1].y = v3Result[(i + 1) % 3].y;

            pRender->DrawLine(ptLine[0], ptLine[1], pBrush);

        }
    }
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
    g_ipTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

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
            SetEvent(g_hChangeResolution);
            Sleep(30);
            continue;
        }

        Render();
    }

    return 0;
}