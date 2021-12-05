#include "pch.h"
#include "CStencilShader.h"
#include <functional>
#include <algorithm>

void CStencilShader::Render(CFace* pFace, CMaterial* pMateril, CLight* pLight, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil)
{
	if (nullptr == m_pStencilBuffer) return;

	XMMATRIX matModel = XMLoadFloat4x4(&m_matModel);
	XMMATRIX matWorld = XMLoadFloat4x4(&m_matWorld);
	XMMATRIX matView = XMLoadFloat4x4(&m_matView);
	XMMATRIX matProj = XMLoadFloat4x4(&m_matProj);
	XMMATRIX matViewport = XMLoadFloat4x4(&m_matViewport);
	XMMATRIX matMWVPV;

	matMWVPV = XMMatrixMultiply(matModel, matWorld);
	matMWVPV = XMMatrixMultiply(matMWVPV, matView);
	matMWVPV = XMMatrixMultiply(matMWVPV, matProj);
	matMWVPV = XMMatrixMultiply(matMWVPV, matViewport);

	VERTEX* pVertices = pFace->GetVertices();
	size_t tVtxCnt = pFace->GetVertexCount();
	XMVECTOR vCalc;


	for (size_t i = 0; i < tVtxCnt; ++i)
	{
		vCalc = XMLoadFloat3(&pVertices[i].v3Vtx);
		vCalc = XMVector3TransformCoord(vCalc, matMWVPV);
		XMStoreFloat3(&pVertices[i].v3Vtx, vCalc);
	}

	if (tVtxCnt == 3)
		DrawTriangle(pVertices, nullptr, nullptr, L"", state, bLight, bZTest, bStencil);
	else
		DrawQuad(pVertices, nullptr, nullptr, L"", state, bLight, bZTest, bStencil);
}

void CStencilShader::DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, CLight* pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil)
{
	const VERTEX* Tri[3];
	Tri[0] = pVertices;
	Tri[1] = pVertices + 1;
	Tri[2] = pVertices + 2;

	if (false == m_bWireframe)
	{
		FillTrilinearInterpolation(nullptr, 0, Tri, pLight, bLight, bZTest, bStencil);
	}
	else
	{
		FillLinearInterpolation(nullptr, 0, pVertices, pVertices + 1, pLight, bLight, bZTest, bStencil);
		FillLinearInterpolation(nullptr, 0, pVertices + 1, pVertices + 2, pLight, bLight, bZTest, bStencil);
		FillLinearInterpolation(nullptr, 0, pVertices + 2, pVertices, pLight, bLight, bZTest, bStencil);

	}
}

void CStencilShader::DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, CLight* pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil)
{
	const VERTEX* TriA[3], * TriB[3];
	TriA[0] = &pVertices[0];
	TriA[1] = &pVertices[1];
	TriA[2] = &pVertices[3];



	TriB[0] = &pVertices[1];
	TriB[1] = &pVertices[2];
	TriB[2] = &pVertices[3];

	if (false == m_bWireframe)
	{
		FillTrilinearInterpolation(nullptr, 0, TriA, pLight, bLight, bZTest, bStencil);
		FillTrilinearInterpolation(nullptr, 0, TriB, pLight, bLight, bZTest, bStencil);

	}
	else
	{

		FillLinearInterpolation(nullptr, 0, TriA[0], TriA[1], pLight, bLight, bZTest, bStencil);
		FillLinearInterpolation(nullptr, 0, TriA[1], TriA[2], pLight, bLight, bZTest, bStencil);
		FillLinearInterpolation(nullptr, 0, TriA[2], TriA[0], pLight, bLight, bZTest, bStencil);

		FillLinearInterpolation(nullptr, 0, TriB[0], TriB[1], pLight, bLight, bZTest, bStencil);
		FillLinearInterpolation(nullptr, 0, TriB[1], TriB[2], pLight, bLight, bZTest, bStencil);
		FillLinearInterpolation(nullptr, 0, TriB[2], TriB[0], pLight, bLight, bZTest, bStencil);

	}

}

void CStencilShader::FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil)
{
	int w = int(m_matViewport._11 * 2.f);
	int h = int(m_matViewport._22 * -2.f);


	const VERTEX* SortedCross[3];
	SortedCross[0] = pVertices[0];
	SortedCross[1] = pVertices[1];
	SortedCross[2] = pVertices[2];

	//cross sort
	const VERTEX* ptTemp;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = i + 1; j < 3; ++j)
		{
			if (SortedCross[i]->v3Vtx.x > SortedCross[j]->v3Vtx.x)
			{
				ptTemp = SortedCross[i];
				SortedCross[i] = SortedCross[j];
				SortedCross[j] = ptTemp;
			}
		}
	}

	CPoint ptA = CPoint(int(roundf(SortedCross[0]->v3Vtx.x)), int(roundf(SortedCross[0]->v3Vtx.y)));
	CPoint ptB = CPoint(int(roundf(SortedCross[1]->v3Vtx.x)), int(roundf(SortedCross[1]->v3Vtx.y)));
	CPoint ptC = CPoint(int(roundf(SortedCross[2]->v3Vtx.x)), int(roundf(SortedCross[2]->v3Vtx.y)));


	CBresenhamLine lineA, lineAB, lineB;
	lineA.Initialize(ptA, ptB);
	lineAB.Initialize(ptB, ptC);
	lineB.Initialize(ptA, ptC);

	typedef struct tagLineDepth
	{
		struct LINE
		{
			long y;
			float z;
			LINE()
			{
				y = 0;
				z = -1;
			}
		};

		LINE line[3];
	}LINE_DEPTH;

	vector<LINE_DEPTH> vecDepth;
	vecDepth.resize(abs(ptA.x - ptC.x) + 1);
	size_t size = vecDepth.size();

	float fRate;

	function<void(CBresenhamLine&, const WORD, const float, const float)> fcLineNextStep = [&](CBresenhamLine& line, const WORD num,  const float zStart, const float zEnd)
	{
		long X = 0;
		long Y = 0;

		float fZ = 0;
		do
		{

			X = line.GetCurrentX();
			Y = line.GetCurrentY();

			fRate = line.GetCurrentRate();
			if (line.isLeft())
				fRate = 1.f - fRate;


			LINE_DEPTH::LINE& dir = vecDepth[X - ptA.x].line[num];

			dir.z = zStart * (1.f - fRate) + zEnd * fRate;

			dir.y = Y;

		} while (line.MoveNext());


	};


	fcLineNextStep(lineB,	2, SortedCross[0]->v3Vtx.z, SortedCross[2]->v3Vtx.z);
	fcLineNextStep(lineA,	0, SortedCross[0]->v3Vtx.z, SortedCross[1]->v3Vtx.z);
	fcLineNextStep(lineAB,	1, SortedCross[1]->v3Vtx.z, SortedCross[2]->v3Vtx.z);

	long x;
	long lCnt = 0;
	long len = 0;
	long lZIndex;
	long lStartY, lEndY, lDirY;
	float fCurrZ, fZStart, fZEnd;

	for (size_t step = 0; step < size; ++step)
	{
		x = long(ptA.x + step);

		if (x < 0 || x >= w) continue;

		//part A
		if (x < ptB.x)
		{
			fZStart = vecDepth[step].line[0].z;
			lStartY = vecDepth[step].line[0].y;
		}
		//part AB
		else
		{
			fZStart = vecDepth[step].line[1].z;
			lStartY = vecDepth[step].line[1].y;
		}

		//part B
		fZEnd = vecDepth[step].line[2].z;
		lEndY = vecDepth[step].line[2].y;

		if (lEndY < lStartY) lDirY = -1;
		else
			lDirY = 1;

		lEndY += lDirY;

		lCnt = 0;
		len = abs(lEndY - lStartY);

		for (long y = lStartY; y != lEndY; y += lDirY)
		{

			if (y < 0 || y >= h) continue;

			if (len == 0)
				fRate = 0;
			else
				fRate = lCnt / (float)len;

			lCnt++;

			fCurrZ = fZStart * (1.f - fRate) + fZEnd * fRate;

			lZIndex = y * w + x;

			if (false == ZTest(m_pStencilBuffer, lZIndex, fCurrZ))
				continue;

		}
	}
}

void CStencilShader::FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil)
{
	int w = int(m_matViewport._11 * 2.f);
	int h = int(m_matViewport._22 * -2.f);

	CPoint ptStart, ptEnd, ptDir;

	ptStart = CPoint((int)roundf(pStart->v3Vtx.x), (int)roundf(pStart->v3Vtx.y));
	ptEnd = CPoint((int)roundf(pEnd->v3Vtx.x), (int)roundf(pEnd->v3Vtx.y));

	if (ptStart.x >= w)
		ptStart.x = w - 1;
	else if (ptStart.x < 0)
		ptStart.x = 0;

	if (ptStart.y >= h)
		ptStart.y = h - 1;
	else if (ptStart.y < 0)
		ptStart.y = 0;

	if (ptEnd.x < 0)
		ptEnd.x = 0;
	else if (ptEnd.x >= w)
		ptEnd.x = w - 1;

	if (ptEnd.y < 0)
		ptEnd.y = 0;
	else if (ptEnd.y >= h)
		ptEnd.y = h - 1;

	ptDir = ptEnd - ptStart;


	CBresenhamLine line;
	line.Initialize(ptStart, ptEnd);

	long x, y;
	float fRate;
	float fZ1 = 1, fZ2 = 1;
	float fZStart = 1, fZEnd = 1;
	float fCurrZ = 1;

	fZStart = pStart->v3Vtx.z;
	fZEnd = pEnd->v3Vtx.z;

	long lZIndex;


	do
	{
		x = line.GetCurrentX();
		y = line.GetCurrentY();

		fRate = line.GetCurrentRate();
		if (line.isLeft())
			fRate = 1.f - fRate;

		//ZBuffer test;

		fZ1 = fZStart * (1.f - fRate);
		fZ2 = fZEnd * fRate;

		fCurrZ = fZ1 + fZ2;

		lZIndex = y * w + x;

		if (false == ZTest(m_pStencilBuffer, lZIndex, fCurrZ))
			continue;

	} while (line.MoveNext());
}
