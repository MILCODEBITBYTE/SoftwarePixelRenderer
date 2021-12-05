#include "pch.h"
#include "CCamera.h"

CCamera::CCamera()
{
    m_v3Front = XMFLOAT3(0, 0, 1);
    m_v3Pos = XMFLOAT3(0, 0, -1);
    m_v3CamPos = XMFLOAT3(0, 0, 0);
    m_v3CamMove = XMFLOAT3(0, 0, 0);
    m_v3Rotate = XMFLOAT3(0, 0, 0);
}

CCamera::~CCamera()
{
}

void CCamera::SetPosition(const XMFLOAT3 &v3Pos)
{
    m_v3CamMove = XMFLOAT3(0, 0, 0);
    m_v3CamPos = v3Pos;
}

void CCamera::SetRotatePitchYawRollFactor(const short shX, const short shY, const short shZ)
{
    //Pitch,Yaw rotate
    if (shY != 0)
    {
        m_v3Rotate.x += shY * 0.02f;
    }
    if (shX != 0)
    {
        m_v3Rotate.y += shX * 0.02f;
    }
    if (shZ != 0)
    {
        m_v3Rotate.z += shZ * 0.02f;
    }

    if (m_v3Rotate.x > XM_PIDIV2) m_v3Rotate.x = XM_PIDIV2;
    else if (m_v3Rotate.x < -XM_PIDIV2) m_v3Rotate.x = -XM_PIDIV2;

    if (m_v3Rotate.y > XM_2PI) m_v3Rotate.y -= XM_2PI;
    else if (m_v3Rotate.y < 0)m_v3Rotate.y += XM_2PI;
    
    if (m_v3Rotate.z > XM_2PI) m_v3Rotate.z -= XM_2PI;
    else if (m_v3Rotate.z < 0) m_v3Rotate.z += XM_2PI;

}


void CCamera::SetPitch(const float fTheta)
{
    m_v3Rotate.x = fTheta;


    if (m_v3Rotate.x > XM_PIDIV2) m_v3Rotate.x = XM_PIDIV2;
    else if (m_v3Rotate.x < -XM_PIDIV2) m_v3Rotate.x = -XM_PIDIV2;
}

void CCamera::SetYaw(const float fTheta)
{
    m_v3Rotate.y = fTheta;

    if (m_v3Rotate.y > XM_2PI) m_v3Rotate.y -= XM_2PI;
    else if (m_v3Rotate.y < 0)m_v3Rotate.y += XM_2PI;
}

void CCamera::SetRoll(const float fTheta)
{
    m_v3Rotate.z = fTheta;

    if (m_v3Rotate.z > XM_2PI) m_v3Rotate.z -= XM_2PI;
    else if (m_v3Rotate.z < 0) m_v3Rotate.z += XM_2PI;
}

void CCamera::Render(const float fEllipse)
{
    XMFLOAT3 v3Front = m_v3Front;
    XMFLOAT3 v3Up = XMFLOAT3(0, 1, 0), v3Right;
    XMVECTOR vF, vU, vR;

    //카메라가 바라보는 전방 백터에서 상향, 우향 백터를 구한다.
    vF = XMLoadFloat3(&v3Front);
    vU = XMLoadFloat3(&v3Up);
    vR = XMVector3Normalize(XMVector3Cross(vU, vF));
    vU = XMVector3Normalize(XMVector3Cross(vF, vR));

    XMStoreFloat3(&v3Up, vU * MOVE_SPEED * fEllipse);
    XMStoreFloat3(&v3Right, vR * MOVE_SPEED * fEllipse);


    //strafe 좌,우 이동
    if (GetAsyncKeyState('A') & 0x8000)
    {
        m_v3CamMove -= v3Right;
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        m_v3CamMove += v3Right;
    }
    if (GetAsyncKeyState('Q') & 0x8000)
    {
        m_v3Rotate.z -= 3 * fEllipse;
    }
    if (GetAsyncKeyState('E') & 0x8000)
    {
        m_v3Rotate.z += 3 * fEllipse;
    }
    
    //전방, 후방 이동
    if (GetAsyncKeyState('W') & 0x8000)
    {
        m_v3CamMove.x += m_v3Front.x * MOVE_SPEED * fEllipse;
        m_v3CamMove.y += m_v3Front.y * MOVE_SPEED * fEllipse;
        m_v3CamMove.z += m_v3Front.z * MOVE_SPEED * fEllipse;
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        m_v3CamMove.x -= m_v3Front.x * MOVE_SPEED * fEllipse;
        m_v3CamMove.y -= m_v3Front.y * MOVE_SPEED * fEllipse;
        m_v3CamMove.z -= m_v3Front.z * MOVE_SPEED * fEllipse;
    }
    //elevation +
    if (GetAsyncKeyState('R') & 0x8000)
    {
        m_v3CamMove += v3Up;
    }
    //elevation -
    if (GetAsyncKeyState('F') & 0x8000)
    {
        m_v3CamMove -= v3Up;
    }

    m_v3Pos = m_v3CamPos + m_v3CamMove;

}

void CCamera::MakeMatrix()
{
    XMFLOAT3 v3Look, v3Up;

    v3Look = XMFLOAT3(0, 0, 1);
    v3Up = XMFLOAT3(0, 1, 0);


    XMMATRIX matRotate = XMMatrixRotationRollPitchYaw(m_v3Rotate.x, m_v3Rotate.y, m_v3Rotate.z);

    XMVECTOR vF, vU, vR;
    vF = XMLoadFloat3(&v3Look);

    //전방 백터
    vF = XMVector3TransformNormal(vF, matRotate);
    XMStoreFloat3(&m_v3Front, vF);

    //상향 백터
    vU = XMLoadFloat3(&v3Up);
    vU = XMVector3TransformNormal(vU, matRotate);

    //우향 백터 
    vR = XMVector3Normalize(XMVector3Cross(vU, vF));

    XMVECTOR vPos = XMVectorNegate(XMLoadFloat3(&m_v3Pos));

    float DotX = XMVectorGetX(XMVector3Dot(vR, vPos));
    float DotY = XMVectorGetX(XMVector3Dot(vU, vPos));
    float DotZ = XMVectorGetX(XMVector3Dot(vF, vPos));

    // right, up, look 순서

    m_matView = XMFLOAT4X4( XMVectorGetX(vR), XMVectorGetX(vU), XMVectorGetX(vF), 0,
                            XMVectorGetY(vR), XMVectorGetY(vU), XMVectorGetY(vF), 0,
                            XMVectorGetZ(vR), XMVectorGetZ(vU), XMVectorGetZ(vF), 0,
                            DotX, DotY, DotZ, 1);
        
}


XMFLOAT4X4 CCamera::GetMatrix() const
{
    return m_matView;
}

XMFLOAT3 CCamera::GetPosition() const
{
    return m_v3Pos;
}

XMFLOAT3 CCamera::GetRotate() const
{
    return m_v3Rotate;
}

XMFLOAT3 CCamera::GetFront() const
{
    return m_v3Front;
}

CRay CCamera::GetCameraRay() const
{
    return m_Ray;
}



bool CCamera::GetKeyState(char Key)
{
    return GetAsyncKeyState(VK_RBUTTON)& 0x8000;
}
