#pragma once

#include "CFace.h"

class CMesh
{
public:
	CMesh();
	CMesh(const CMesh& mesh);
	virtual ~CMesh();

	virtual void Initialize();
	bool LoadFromFile(const wchar_t* szFileName);

	XMFLOAT3*	GetVertices();
	size_t	GetVertexCount();
	DWORD*	GetIndices();
	size_t	GetIndexCount();



protected:
	virtual void MakeMesh();


protected:
	vector<XMFLOAT3>	m_vecVertices;
	vector<DWORD>		m_vecIndices;
};

