#include "pch.h"
#include "CBoundbox.h"

CBoundbox::CBoundbox()
{
	m_Type = OBJECT_TYPE::BOUNDBOX;
	m_Box.min = XMFLOAT3(0, 0, 0);
	m_Box.max = XMFLOAT3(0, 0, 0);
}

CBoundbox::CBoundbox(const CBoundbox& box)
	:CMesh(box)
{
	m_Box = box.m_Box;
}

CBoundbox::~CBoundbox()
{
}

CBoundbox* CBoundbox::Clone()
{
	return new CBoundbox(*this);
}

void CBoundbox::CalcBound(VERTEX* pVertices, const size_t tVtxCount)
{
	for (size_t i = 0; i < tVtxCount; ++i)
	{
		if (pVertices[i].v3Vtx.x < FLT_EPSILON)
		{
			if (m_Box.min.x > pVertices[i].v3Vtx.x)
				m_Box.min.x = pVertices[i].v3Vtx.x;
		}
		else
		{
			if (m_Box.max.x < pVertices[i].v3Vtx.x)
				m_Box.max.x = pVertices[i].v3Vtx.x;
		}

		if (pVertices[i].v3Vtx.y < FLT_EPSILON)
		{
			if (m_Box.min.y > pVertices[i].v3Vtx.y)
				m_Box.min.y = pVertices[i].v3Vtx.y;
		}
		else
		{
			if (m_Box.max.y < pVertices[i].v3Vtx.y)
				m_Box.max.y = pVertices[i].v3Vtx.y;
		}

		if (pVertices[i].v3Vtx.z < FLT_EPSILON)
		{
			if (m_Box.min.z > pVertices[i].v3Vtx.z)
				m_Box.min.z = pVertices[i].v3Vtx.z;
		}
		else
		{
			if (m_Box.max.z < pVertices[i].v3Vtx.z)
				m_Box.max.z = pVertices[i].v3Vtx.z;
		}
	}
}

void CBoundbox::CalcBound(VERTEX** ppVertices, const size_t tVtxCount)
{
	for (size_t i = 0; i < tVtxCount; ++i)
	{
		if (ppVertices[i]->v3Vtx.x < FLT_EPSILON)
		{
			if (m_Box.min.x > ppVertices[i]->v3Vtx.x)
				m_Box.min.x = ppVertices[i]->v3Vtx.x;
		}
		else
		{
			if (m_Box.max.x < ppVertices[i]->v3Vtx.x)
				m_Box.max.x = ppVertices[i]->v3Vtx.x;
		}

		if (ppVertices[i]->v3Vtx.y < FLT_EPSILON)
		{
			if (m_Box.min.y > ppVertices[i]->v3Vtx.y)
				m_Box.min.y = ppVertices[i]->v3Vtx.y;
		}
		else
		{
			if (m_Box.max.y < ppVertices[i]->v3Vtx.y)
				m_Box.max.y = ppVertices[i]->v3Vtx.y;
		}

		if (ppVertices[i]->v3Vtx.z < FLT_EPSILON)
		{
			if (m_Box.min.z > ppVertices[i]->v3Vtx.z)
				m_Box.min.z = ppVertices[i]->v3Vtx.z;
		}
		else
		{
			if (m_Box.max.z < ppVertices[i]->v3Vtx.z)
				m_Box.max.z = ppVertices[i]->v3Vtx.z;
		}
	}

}

void CBoundbox::Reset()
{
	m_Box.min = XMFLOAT3(0, 0, 0);
	m_Box.max = XMFLOAT3(0, 0, 0);
}

void CBoundbox::GetBoundMinMax(BOUND_MIN_MAX *pBox)
{
	*pBox = m_Box;
}

void CBoundbox::MakeVolume()
{
	m_vecModels.clear();
	m_vecModels.push_back(MESH_MODEL());

	MESH_MODEL* pModel = &m_vecModels.back();
	pModel->strName = L"Boundbox";

	vector<VERTEX> &vecVertices = pModel->vecVertices;
	vecVertices.resize(8);
	vector<CFace>& vecFacies = pModel->vecFacies;
	vector<DWORD> vecIndices;

	/*
	*       0                     1
	*          式式式式式式式式式
	*        ㄞ弛               ㄞ弛
	*      ㄞ  弛    Up       ㄞ  弛
	*    ㄞ    弛           ㄞ	  弛
	*  ㄞ	   弛	   Ba ㄞ      弛
	  3	式式式式式式式式式 2      弛
	  弛       弛        弛       弛
	  弛   L   弛        弛   R   弛
	  弛      4  式式式式弛式式式 5
	  弛       ㄞFr      弛      ㄞ
	  弛     ㄞ          弛    ㄞ
	  弛   ㄞ      Bo    弛  ㄞ
	  弛 ㄞ              弛ㄞ
	  7 式式式式式式式式式 6


	*/

	//upper rectangle
	vecVertices[0].v3Vtx.x = m_Box.min.x;
	vecVertices[0].v3Vtx.y = m_Box.max.y;
	vecVertices[0].v3Vtx.z = m_Box.max.z;
	vecVertices[1].v3Vtx.x = m_Box.max.x;
	vecVertices[1].v3Vtx.y = m_Box.max.y;
	vecVertices[1].v3Vtx.z = m_Box.max.z;
	vecVertices[2].v3Vtx.x = m_Box.max.x;
	vecVertices[2].v3Vtx.y = m_Box.max.y;
	vecVertices[2].v3Vtx.z = m_Box.min.z;
	vecVertices[3].v3Vtx.x = m_Box.min.x;
	vecVertices[3].v3Vtx.y = m_Box.max.y;
	vecVertices[3].v3Vtx.z = m_Box.min.z;

	//bottom rectangle
	vecVertices[4].v3Vtx.x = m_Box.min.x;
	vecVertices[4].v3Vtx.y = m_Box.min.y;
	vecVertices[4].v3Vtx.z = m_Box.max.z;
	vecVertices[5].v3Vtx.x = m_Box.max.x;
	vecVertices[5].v3Vtx.y = m_Box.min.y;
	vecVertices[5].v3Vtx.z = m_Box.max.z;
	vecVertices[6].v3Vtx.x = m_Box.max.x;
	vecVertices[6].v3Vtx.y = m_Box.min.y;
	vecVertices[6].v3Vtx.z = m_Box.min.z;
	vecVertices[7].v3Vtx.x = m_Box.min.x;
	vecVertices[7].v3Vtx.y = m_Box.min.y;
	vecVertices[7].v3Vtx.z = m_Box.min.z;

	//triangle faces
	vecIndices.resize(3 * 2 * 6);

	//upper face
	vecIndices[0] = 0;
	vecIndices[1] = 1;
	vecIndices[2] = 3;
	vecIndices[3] = 1;
	vecIndices[4] = 2;
	vecIndices[5] = 3;


	//7    6

	//4    5

	//bottom face
	vecIndices[6] = 7;
	vecIndices[7] = 6;
	vecIndices[8] = 4;
	vecIndices[9] = 6;
	vecIndices[10] = 5;
	vecIndices[11] = 4;

	// 3    2

	// 7    6

	//front face
	vecIndices[12] = 3;
	vecIndices[13] = 2;
	vecIndices[14] = 7;
	vecIndices[15] = 2;
	vecIndices[16] = 6;
	vecIndices[17] = 7;


	// 4      5

	//  0      1

	//back face
	vecIndices[18] = 4;
	vecIndices[19] = 5;
	vecIndices[20] = 0;
	vecIndices[21] = 5;
	vecIndices[22] = 1;
	vecIndices[23] = 0;

	// 0  3

	//  4  7

	//left face
	vecIndices[24] = 0;
	vecIndices[25] = 3;
	vecIndices[26] = 4;
	vecIndices[27] = 3;
	vecIndices[28] = 7;
	vecIndices[29] = 4;

	//  2   1

	//  6   5
	//right face
	vecIndices[30] = 2;
	vecIndices[31] = 1;
	vecIndices[32] = 6;
	vecIndices[33] = 1;
	vecIndices[34] = 5;
	vecIndices[35] = 6;

	vecFacies.resize(vecIndices.size() / 3);

	//input face
	for (size_t i = 0; i < vecIndices.size(); i += 3)
	{
		CFace* pFace = &vecFacies[i / 3];

		pFace->SetFace(vecVertices[vecIndices[i + 0]], vecVertices[vecIndices[i + 1]], vecVertices[vecIndices[i + 2]]);
		pFace->MakeNormal();
	}

}
