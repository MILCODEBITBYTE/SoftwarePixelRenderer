#pragma once

#include "CRay.h"

const float MOVE_SPEED = 7;

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

	void	SetPosition(const XMFLOAT3 &v3Pos);
	void	SetRotatePitchYawRollFactor(const short shX, const short shY, const short shZ);
	void	SetPitch(const float fTheta);
	void	SetYaw(const float fTheta);
	void	SetRoll(const float fTheta);

	virtual void	Render(const float fEllipse);
	virtual void	MakeMatrix();

	XMFLOAT4X4	GetMatrix() const;
	XMFLOAT3	GetPosition() const;
	XMFLOAT3	GetRotate() const;
	XMFLOAT3	GetFront() const;

	CRay		GetCameraRay() const;

protected:
	bool GetKeyState(char Key);

protected:
	XMFLOAT4X4 m_matView;
	XMFLOAT3	m_v3Pos;
	XMFLOAT3	m_v3CamPos;
	XMFLOAT3	m_v3CamMove;
	XMFLOAT3	m_v3Rotate;
	XMFLOAT3	m_v3Front;

	CRay		m_Ray;
};

