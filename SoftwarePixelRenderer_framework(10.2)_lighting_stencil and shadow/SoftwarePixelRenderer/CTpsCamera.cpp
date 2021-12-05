#include "pch.h"
#include "CTpsCamera.h"

CTpsCamera::CTpsCamera()
{
    m_fZoomFactor = 2;
}

CTpsCamera::~CTpsCamera()
{
}

void CTpsCamera::SetZoom(const float iFactor)
{
    m_fZoomFactor += iFactor;

    if (m_fZoomFactor < 2)
        m_fZoomFactor = 2;
}


void CTpsCamera::Render(const float fEllipse)
{
    CFpsCamera::Render(fEllipse);

    //zoom in, out
    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        m_fZoomFactor -= 1 * fEllipse * MOVE_SPEED;

    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        m_fZoomFactor += 1 * fEllipse * MOVE_SPEED;
    }
    if (m_fZoomFactor < 2) m_fZoomFactor = 2;
    
}

void CTpsCamera::MakeMatrix()
{
    CCamera::MakeMatrix();

    //�ٶ󺸴� �������� zoom, out �̵���� ����
    XMFLOAT3 v3Dist = XMFLOAT3(m_v3Front.x * m_fZoomFactor,
        m_v3Front.y * m_fZoomFactor,
        m_v3Front.z * m_fZoomFactor);

    XMMATRIX matResult, matView, matTrans;
    
    matView = XMLoadFloat4x4(&m_matView);

    //�� ��İ� �ռ�
    matTrans = XMMatrixTranslation(v3Dist.x, v3Dist.y, v3Dist.z);

    matResult = XMMatrixMultiply(matTrans, matView);

    XMStoreFloat4x4(&m_matView, matResult);

}
