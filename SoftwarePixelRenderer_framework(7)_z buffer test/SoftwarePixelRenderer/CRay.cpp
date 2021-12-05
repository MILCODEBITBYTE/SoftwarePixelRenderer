#include "pch.h"
#include "CRay.h"

CRay::CRay()
{
}

CRay::~CRay()
{
}

void CRay::SetOrigin(const XMFLOAT3& v3Org)
{
	m_v3Origin = v3Org;
}

void CRay::SetDirection(const XMFLOAT3& v3Dir)
{
	m_v3Direction = v3Dir;
}

XMFLOAT3 CRay::GetOrigin() const
{
	return m_v3Origin;
}

XMFLOAT3 CRay::GetDirection() const
{
	return m_v3Direction;
}
