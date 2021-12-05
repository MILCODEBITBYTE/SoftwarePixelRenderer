#include "pch.h"
#include "CFpsCamera.h"



CFpsCamera::CFpsCamera()
{
}

CFpsCamera::~CFpsCamera()
{
}

void CFpsCamera::Render(const float fEllipse)
{
    XMFLOAT3 v3Front = XMFLOAT3(0, 0, 1), v3Look;
    XMFLOAT3 v3Up = XMFLOAT3(0, 1, 0), v3Right;
    XMVECTOR vF, vU, vR;

    //ī�޶� �ٶ󺸴� ���� ���Ϳ��� ����, ���� ���͸� ���Ѵ�.
    vF = XMLoadFloat3(&v3Front);

    XMMATRIX matYaw = XMMatrixRotationY(m_v3Rotate.y);
    vF = XMVector3TransformNormal(vF, matYaw);

    XMStoreFloat3(&v3Look, vF);

    vU = XMLoadFloat3(&v3Up);
    vR = XMVector3Normalize(XMVector3Cross(vU, vF));
    

    XMStoreFloat3(&v3Up, vU * MOVE_SPEED * fEllipse);
    XMStoreFloat3(&v3Right, vR * MOVE_SPEED * fEllipse);



    //ī�޶� �̵��� v3Front �������� ȸ��(YAW)  �̵�

    //strafe ��,�� �̵�
    if (GetAsyncKeyState('A') & 0x8000)
    {
        m_v3CamMove -= v3Right;
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        m_v3CamMove += v3Right;
    }
    //����, �Ĺ� �̵�
    if (GetAsyncKeyState('W') & 0x8000)
    {
        m_v3CamMove.x += v3Look.x * MOVE_SPEED * fEllipse;
        m_v3CamMove.y += v3Look.y * MOVE_SPEED * fEllipse;
        m_v3CamMove.z += v3Look.z * MOVE_SPEED * fEllipse;
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        m_v3CamMove.x -= v3Look.x * MOVE_SPEED * fEllipse;
        m_v3CamMove.y -= v3Look.y * MOVE_SPEED * fEllipse;
        m_v3CamMove.z -= v3Look.z * MOVE_SPEED * fEllipse;
    }
    //elevation +
    if (GetAsyncKeyState('R') & 0x8000)
    {
        m_v3CamPos.y += MOVE_SPEED * fEllipse;
    }
    //elevation -
    if (GetAsyncKeyState('F') & 0x8000)
    {
        m_v3CamPos.y -= MOVE_SPEED * fEllipse;
    }


    m_v3Pos = m_v3CamPos + m_v3CamMove;


}
