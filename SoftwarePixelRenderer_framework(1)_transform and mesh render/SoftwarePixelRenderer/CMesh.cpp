#include "pch.h"
#include "CMesh.h"

CMesh::CMesh()
{
}

CMesh::CMesh(const CMesh& mesh)
{
    m_vecVertices = mesh.m_vecVertices;
    m_vecIndices = mesh.m_vecIndices;
}

CMesh::~CMesh()
{
}

void CMesh::Initialize()
{
}

bool CMesh::LoadFromFile(const wchar_t* szFileName)
{
    return false;
}

XMFLOAT3* CMesh::GetVertices()
{
    return m_vecVertices.data();
}

size_t CMesh::GetVertexCount()
{
    return m_vecVertices.size();
}

DWORD* CMesh::GetIndices()
{
    return m_vecIndices.data();
}

size_t CMesh::GetIndexCount()
{
    return m_vecIndices.size();
}

void CMesh::MakeMesh()
{
}
