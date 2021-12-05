#include "pch.h"
#include "CFace.h"

CFace::CFace()
{
	m_iFaceInVertexCount = 0;
}

CFace::CFace(const VERTEX& vtx1, const VERTEX& vtx2, const VERTEX& vtx3)
{
	m_iFaceInVertexCount = 3;
	m_vecVertices.resize(3);

	m_vecVertices[0] = vtx1;
	m_vecVertices[1] = vtx2;
	m_vecVertices[2] = vtx3;
}

CFace::CFace(const VERTEX& vtx1, const VERTEX& vtx2, const VERTEX& vtx3, const VERTEX& vtx4)
{
	m_iFaceInVertexCount = 4;
	m_vecVertices.resize(4);

	m_vecVertices[0] = vtx1;
	m_vecVertices[1] = vtx2;
	m_vecVertices[2] = vtx3;
	m_vecVertices[3] = vtx4;
}


CFace::~CFace()
{
}

void CFace::SetFace(const VERTEX& vtx1, const VERTEX& vtx2, const VERTEX& vtx3)
{
	if (m_iFaceInVertexCount != 3)
	{
		m_vecVertices.resize(3);
		m_iFaceInVertexCount = 3;
	}

	m_vecVertices[0] = vtx1;
	m_vecVertices[1] = vtx2;
	m_vecVertices[2] = vtx3;
}

void CFace::SetFace(const VERTEX& vtx1, const VERTEX& vtx2, const VERTEX& vtx3, const VERTEX& vtx4)
{
	if (m_iFaceInVertexCount != 4)
	{
		m_vecVertices.resize(4);
		m_iFaceInVertexCount = 4;
	}

	m_vecVertices[0] = vtx1;
	m_vecVertices[1] = vtx2;
	m_vecVertices[2] = vtx3;
	m_vecVertices[3] = vtx4;

}

size_t CFace::GetVertexCount() const
{
	return m_vecVertices.size();
}

VERTEX* CFace::GetVertices()
{
	return m_vecVertices.data();
}

void CFace::MakeNormal()
{
	XMVECTOR p1, p2;
	XMFLOAT3 v3Dir1, v3Dir2;
	XMFLOAT3 v3Normal;
	v3Dir1 = m_vecVertices[0].v3Vtx  - m_vecVertices[1].v3Vtx;
	v3Dir2 = m_vecVertices[2].v3Vtx - m_vecVertices[1].v3Vtx;
	p1 = XMLoadFloat3(&v3Dir1);
	p2 = XMLoadFloat3(&v3Dir2);

	XMStoreFloat3(&v3Normal, XMVector3Normalize(XMVector3Cross(p1, p2)));

	for (size_t i = 0; i < m_vecVertices.size(); ++i)
	{
		m_vecVertices[i].v3Normal = v3Normal;
	}

}

void CFace::GetNormal(XMVECTOR *pNormal)
{
	*pNormal = XMLoadFloat3(&m_vecVertices[0].v3Normal);
}

void CFace::GetNormal(XMFLOAT3* pNormal)
{
	*pNormal = m_vecVertices[0].v3Normal;
}

float CFace::CalcVectorDotCosf(const XMVECTOR& v)
{
	XMFLOAT3 v32;

	XMFLOAT3 &v3Normal = m_vecVertices[0].v3Normal;

	XMStoreFloat3(&v32, v);
	return acosf(v3Normal.x * v32.x + v3Normal.y * v32.y + v3Normal.z * v32.z);
}

float CFace::CalcVectorDotCosf(const XMVECTOR& vNormal, const XMVECTOR& vLook)
{
	XMFLOAT3 v3Normal, v3Look;

	XMStoreFloat3(&v3Normal, vNormal);
	XMStoreFloat3(&v3Look, vLook);

	return acosf(v3Normal.x * v3Look.x + v3Normal.y * v3Look.y + v3Normal.z * v3Look.z);
}

float CFace::CalcVectorDotCosf(const XMFLOAT3& v3)
{
	XMFLOAT3& v3Normal = m_vecVertices[0].v3Normal;

	return acosf(v3Normal.x * v3.x + v3Normal.y * v3.y + v3Normal.z * v3.z);
}
