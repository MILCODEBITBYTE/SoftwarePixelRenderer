#include "pch.h"
#include "CFace.h"

CFace::CFace()
{
}

CFace::CFace(const XMFLOAT3& v31, const XMFLOAT3& v32, const XMFLOAT3& v33)
{
	m_v31 = v31;
	m_v32 = v32;
	m_v33 = v33;
}

CFace::~CFace()
{
}

void CFace::SetFace(const XMFLOAT3& v31, const XMFLOAT3& v32, const XMFLOAT3& v33)
{
	m_v31 = v31;
	m_v32 = v32;
	m_v33 = v33;
}

void CFace::MakeNormal()
{
	XMVECTOR p1, p2;
	XMFLOAT3 v3Dir1, v3Dir2;
	v3Dir1 = m_v31 - m_v32;
	v3Dir2 = m_v33 - m_v32;
	p1 = XMLoadFloat3(&v3Dir1);
	p2 = XMLoadFloat3(&v3Dir2);

	XMStoreFloat3(&m_v3Normal, XMVector3Normalize(XMVector3Cross(p1, p2)));
}

void CFace::GetNormal(XMVECTOR *pNormal)
{
	*pNormal = XMLoadFloat3(&m_v3Normal);
}

void CFace::GetNormal(XMFLOAT3* pNormal)
{
	*pNormal = m_v3Normal;
}

float CFace::CalcVectorDotCosf(const XMVECTOR& v)
{
	XMFLOAT3 v32;

	XMStoreFloat3(&v32, v);
	return acosf(m_v3Normal.x * v32.x + m_v3Normal.y * v32.y + m_v3Normal.z * v32.z);
}

float CFace::CalcVectorDotCosf(const XMFLOAT3& v3)
{
	return acosf(m_v3Normal.x * v3.x + m_v3Normal.y * v3.y + m_v3Normal.z * v3.z);
}
