#include "pch.h"
#include "CVertexColorShader.h"
#include <algorithm>
#include <functional>

CVertexColorShader::CVertexColorShader()
{
}

CVertexColorShader::~CVertexColorShader()
{
}

void CVertexColorShader::Render(CFace* pFace, CMaterial* pMaterial, const RENDER_STATE state)
{
	if (nullptr == m_pScreenBuffer) return;

	int w = int(m_matViewport._11 * 2.f);
	int h = int(m_matViewport._22 * -2.f);

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
		DrawTriangle(pVertices, nullptr, pFace->GetMaterialName(), w, h, state);
	else
		DrawQuad(pVertices,nullptr, pFace->GetMaterialName(), w, h, state);
}


void CVertexColorShader::DrawTriTest(BYTE* pBack, const UINT stride, const VERTEX* pVertices, const int iScreenWidth, const int iScreenHeight)
{
	XMFLOAT4 v4Col;
	v4Col.x = (pVertices[0].v4Color.x + pVertices[1].v4Color.x + pVertices[2].v4Color.x) / 3.f;
	v4Col.y = (pVertices[0].v4Color.y + pVertices[1].v4Color.y + pVertices[2].v4Color.y) / 3.f;
	v4Col.z = (pVertices[0].v4Color.z + pVertices[1].v4Color.z + pVertices[2].v4Color.z) / 3.f;
	v4Col.w = (pVertices[0].v4Color.w + pVertices[1].v4Color.w + pVertices[2].v4Color.w) / 3.f;

	for (int i = 0; i < 3; ++i)
	{
		//bresenham algorithm
		DrawFlatLine(pBack, stride, v4Col, pVertices, pVertices + 1 , iScreenWidth, iScreenHeight);
		DrawFlatLine(pBack, stride, v4Col, pVertices+1, pVertices + 2, iScreenWidth, iScreenHeight);
		DrawFlatLine(pBack, stride, v4Col, pVertices+2, pVertices,  iScreenWidth, iScreenHeight);
	}
}

void CVertexColorShader::FillTriTest(BYTE* pBack, const UINT stride, const VERTEX* pVertices, const int iScreenWidth, const int iScreenHeight)
{
	const VERTEX* SortedCross[3];
	SortedCross[0] = &pVertices[0];
	SortedCross[1] = &pVertices[1];
	SortedCross[2] = &pVertices[2];

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

	XMFLOAT4 v4Col;
	v4Col.x = (pVertices[0].v4Color.x + pVertices[1].v4Color.x + pVertices[2].v4Color.x) / 3.f;
	v4Col.y = (pVertices[0].v4Color.y + pVertices[1].v4Color.y + pVertices[2].v4Color.y) / 3.f;
	v4Col.z = (pVertices[0].v4Color.z + pVertices[1].v4Color.z + pVertices[2].v4Color.z) / 3.f;
	v4Col.w = (pVertices[0].v4Color.w + pVertices[1].v4Color.w + pVertices[2].v4Color.w) / 3.f;

	BYTE R, G, B, A;
	R = (BYTE)(v4Col.x * 255.f);
	G = (BYTE)(v4Col.y * 255.f);
	B = (BYTE)(v4Col.z * 255.f);
	A = (BYTE)(v4Col.w * 255.f);

	typedef struct tagLineDepth
	{
		struct LINE
		{
			long y;
			LINE()
			{
				y = 0;
			}
		};

		LINE line[3];
	}LINE_DEPTH;

	vector<LINE_DEPTH> vecDepth;
	vecDepth.resize(abs(ptA.x - ptC.x) + 1);
	size_t size = vecDepth.size();


	function<void(CBresenhamLine&, const WORD)> fcLineNextStep = [&](CBresenhamLine& line, const WORD num)
	{
		long X = 0;
		long Y = 0;
		do
		{

			X = line.GetCurrentX();
			Y = line.GetCurrentY();

			LINE_DEPTH::LINE& dir = vecDepth[X - ptA.x].line[num];

			dir.y = Y;

		} while (line.MoveNext());


	};

	fcLineNextStep(lineB, 2);
	fcLineNextStep(lineA, 0);
	fcLineNextStep(lineAB, 1);

	long x;
	long lStartY, lEndY, lDirY;
	long lBackIndex;

	long len = 0;
	long lCnt = 0;
	//draw top down line

	for (size_t step = 0; step < size; ++step)
	{
		x = long(ptA.x + step);

		if (x < 0 || x >= iScreenWidth) continue;

		//part A
		if (x < ptB.x)
		{
			lStartY = vecDepth[step].line[0].y;
		}
		//part AB
		else
		{
			lStartY = vecDepth[step].line[1].y;
		}
		//part B
		lEndY = vecDepth[step].line[2].y;

		if (lEndY < lStartY) lDirY = -1;
		else
			lDirY = 1;

		lEndY += lDirY;

		lCnt = 0;
		len = abs(lStartY - lEndY);
		for (long y = lStartY; y != lEndY; y += lDirY)
		{
			if (y < 0 || y >= iScreenHeight) continue;

			lBackIndex = y * stride + x * 4;

			pBack[lBackIndex + 0] = B;
			pBack[lBackIndex + 1] = G;
			pBack[lBackIndex + 2] = R;
			pBack[lBackIndex + 3] = A;
		}
	}

}



void CVertexColorShader::DrawTriSmooth(BYTE* pBack, const UINT stride, const VERTEX* pVertices, const int iScreenWidth, const int iScreenHeight)
{
	for (int i = 0; i < 3; ++i)
	{
		DrawSmoothLine(pBack, stride, pVertices, pVertices + 1,  iScreenWidth, iScreenHeight);
		DrawSmoothLine(pBack, stride, pVertices + 1, pVertices + 2, iScreenWidth, iScreenHeight);
		DrawSmoothLine(pBack, stride, pVertices + 2, pVertices,  iScreenWidth, iScreenHeight);
	}
}

void CVertexColorShader::FillTriSmooth(BYTE* pBack, const UINT stride, const VERTEX* pVertices, const int iScreenWidth, const int iScreenHeight)
{
	const VERTEX* SortedCross[3];
	SortedCross[0] = &pVertices[0];
	SortedCross[1] = &pVertices[1];
	SortedCross[2] = &pVertices[2];

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

	XMFLOAT4 v4Col;

	typedef struct tagLineDepth
	{
		struct LINE
		{
			long y;
			XMFLOAT4 col;
			LINE()
			{
				y = 0;
			}
		};

		LINE line[3];
	}LINE_DEPTH;

	vector<LINE_DEPTH> vecDepth;
	vecDepth.resize(abs(ptA.x - ptC.x) + 1);
	size_t size = vecDepth.size();

	float fRate;

	function<void(CBresenhamLine&, const WORD, const XMFLOAT4, const XMFLOAT4)> fcLineNextStep = [&](CBresenhamLine& line, const WORD num, const XMFLOAT4 colStart, const XMFLOAT4 colEnd)
	{
		long X = 0;
		long Y = 0;

		do
		{

			X = line.GetCurrentX();
			Y = line.GetCurrentY();

			fRate = line.GetCurrentRate();
			if (line.isLeft())
				fRate = 1.f - fRate;


			LINE_DEPTH::LINE& dir = vecDepth[X - ptA.x].line[num];

			dir.col.x = colStart.x * (1.f - fRate) + colEnd.x * fRate;
			dir.col.y = colStart.y * (1.f - fRate) + colEnd.y * fRate;
			dir.col.z = colStart.z * (1.f - fRate) + colEnd.z * fRate;
			dir.col.w = colStart.w * (1.f - fRate) + colEnd.w * fRate;

			dir.y = Y;

		} while (line.MoveNext());


	};


	fcLineNextStep(lineB, 2, SortedCross[0]->v4Color, SortedCross[2]->v4Color);
	fcLineNextStep(lineA, 0, SortedCross[0]->v4Color, SortedCross[1]->v4Color);
	fcLineNextStep(lineAB, 1, SortedCross[1]->v4Color, SortedCross[2]->v4Color);

	long x;
	long lCnt = 0;
	long len = 0;
	long lBackIndex;
	long lStartY, lEndY, lDirY;
	XMFLOAT4 colStart, colEnd;

	for (size_t step = 0; step < size; ++step)
	{
		x = long(ptA.x + step);

		if (x < 0 || x >= iScreenWidth) continue;

		//part A
		if (x < ptB.x)
		{
			lStartY = vecDepth[step].line[0].y;
			colStart = vecDepth[step].line[0].col;
		}
		//part AB
		else
		{
			lStartY = vecDepth[step].line[1].y;
			colStart = vecDepth[step].line[1].col;
		}

		//part B
		lEndY = vecDepth[step].line[2].y;
		colEnd = vecDepth[step].line[2].col;

		if (lEndY < lStartY) lDirY = -1;
		else
			lDirY = 1;

		lEndY += lDirY;

		lCnt = 0;
		len = abs(lEndY - lStartY);

		for (long y = lStartY; y != lEndY; y += lDirY)
		{

			if (y < 0 || y >= iScreenHeight) continue;

			if (len == 0)
				fRate = 0;
			else
				fRate = lCnt / (float)len;

			lCnt++;

			v4Col.x = colStart.x * (1.f - fRate) + colEnd.x * fRate;
			v4Col.y = colStart.y * (1.f - fRate) + colEnd.y * fRate;
			v4Col.z = colStart.z * (1.f - fRate) + colEnd.z * fRate;
			v4Col.w = colStart.w * (1.f - fRate) + colEnd.w * fRate;

			lBackIndex = y * stride + x * 4;

			pBack[lBackIndex + 0] = BYTE(v4Col.z * 255.f);
			pBack[lBackIndex + 1] = BYTE(v4Col.y * 255.f);
			pBack[lBackIndex + 2] = BYTE(v4Col.x * 255.f);
			pBack[lBackIndex + 3] = BYTE(v4Col.w * 255.f);
		}
	}
}

void CVertexColorShader::DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const int iScreenWidth, const int iScreenHeight, const RENDER_STATE state)
{
	if (nullptr == m_pScreenBuffer) return;

	HRESULT hr;
	IWICBitmapLock* pBackLock = nullptr;
	UINT uiBackW, uiBackH;

	WICRect rcBack;

	m_pScreenBuffer->GetSize(&uiBackW, &uiBackH);
	rcBack.X = 0;
	rcBack.Y = 0;
	rcBack.Width = uiBackW;
	rcBack.Height = uiBackH;



	/*                ／ v2  Y cross
	*                ／｜
	*               ／ ｜
	*              ／  ｜
	*		      ／   ｜
				 ／    ｜
				／     ｜
			   ／  (A) ｜	
			  ／       ｜
			v1----------> X cross (center)
              ＼       ｜
				＼ (B) ｜
				   ＼  ｜ 
				     ＼｜
						v3

		정점 x값이 제일 작은순으로 정렬된 삼각형을 가운데 정점 기준으로 
		A와 B로 나누어 각각 픽셀을 찍는다.
	*/




	hr = m_pScreenBuffer->Lock(&rcBack, WICBitmapLockWrite, &pBackLock);

	if (SUCCEEDED(hr))
	{
		BYTE* pBackData;
		UINT uiBackSize;
		UINT uiBackStride;

		hr = pBackLock->GetDataPointer(&uiBackSize, &pBackData);
		if (SUCCEEDED(hr))
		{
			pBackLock->GetStride(&uiBackStride);

			if (state == RENDER_STATE::BOUNDBOX)
			{
				DrawTriTest(pBackData, uiBackStride, pVertices, iScreenWidth, iScreenHeight);
			}

			else if (state == RENDER_STATE::TEST)
			{
				if (false == m_bWireframe)
				{
					FillTriTest(pBackData, uiBackStride, pVertices, iScreenWidth, iScreenHeight);
				}
				else
				{
					DrawTriTest(pBackData, uiBackStride, pVertices, iScreenWidth, iScreenHeight);
				}
			}
				
			else if (state == RENDER_STATE::FLAT)
			{
				if (false == m_bWireframe)
				{
					FillTriSmooth(pBackData, uiBackStride, pVertices, iScreenWidth, iScreenHeight);
				}
				else
				{
					DrawTriSmooth(pBackData, uiBackStride, pVertices, iScreenWidth, iScreenHeight);
				}
			}
				
		}

		pBackLock->Release();
	}
}

void CVertexColorShader::DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const int iScreenWidth, const int iScreenHeight, const RENDER_STATE state)
{
	if (nullptr == m_pScreenBuffer) return;

	HRESULT hr;
	IWICBitmapLock* pBackLock = nullptr;
	UINT uiBackW, uiBackH;

	WICRect rcBack;

	m_pScreenBuffer->GetSize(&uiBackW, &uiBackH);
	rcBack.X = 0;
	rcBack.Y = 0;
	rcBack.Width = uiBackW;
	rcBack.Height = uiBackH;



	/* 
	 
	            -> cross A
	 
	           v2-------------------v3
			   /＼	 			    /
	    	  /   ＼	B		   /
	 ↓    	 /	    ＼			  /
	cross b	/		  ＼		 /
		   /	A	    ＼		/
	      /				  ＼   /
	     /					＼/
	   v1------------------v4
	 
	 
	 
		정점 x값이 제일 작은순으로 정렬된 사각형을 가운데 두 정점 v2, v4을 기준으로
	    삼각형 A와 B로 나누어 각각 찍는다.
	*/




	hr = m_pScreenBuffer->Lock(&rcBack, WICBitmapLockWrite, &pBackLock);

	if (SUCCEEDED(hr))
	{
		BYTE* pBackData;
		UINT uiBackSize;
		UINT uiBackStride;

		hr = pBackLock->GetDataPointer(&uiBackSize, &pBackData);
		if (SUCCEEDED(hr))
		{
			pBackLock->GetStride(&uiBackStride);

			VERTEX* SortedCross[4];
			for (int i = 0; i < 4; ++i)
				SortedCross[i] = pVertices + i;

			if (state == RENDER_STATE::FLAT)
			{
				XMFLOAT4 v4Col;
				v4Col.x = (SortedCross[0]->v4Color.x + SortedCross[1]->v4Color.x + SortedCross[2]->v4Color.x + SortedCross[3]->v4Color.x) / 4.f;
				v4Col.y = (SortedCross[0]->v4Color.y + SortedCross[1]->v4Color.y + SortedCross[2]->v4Color.y + SortedCross[3]->v4Color.y) / 4.f;
				v4Col.z = (SortedCross[0]->v4Color.z + SortedCross[1]->v4Color.z + SortedCross[2]->v4Color.z + SortedCross[3]->v4Color.z) / 4.f;

				for (int i = 0; i < 4; ++i)
					SortedCross[i]->v4Color = v4Col;
			}

			VERTEX TriA[3], TriB[3];
			TriA[0] = *SortedCross[0];
			TriA[1] = *SortedCross[1];
			TriA[2] = *SortedCross[3];

		

			TriB[0] = *SortedCross[1];
			TriB[1] = *SortedCross[2];
			TriB[2] = *SortedCross[3];

			if (state == RENDER_STATE::TEST)
			{
				if (false == m_bWireframe)
				{
					FillTriTest(pBackData, uiBackStride, TriA, iScreenWidth, iScreenHeight);
					FillTriTest(pBackData, uiBackStride, TriB, iScreenWidth, iScreenHeight);
				}
				else
				{
					DrawTriTest(pBackData, uiBackStride, TriA, iScreenWidth, iScreenHeight);
					DrawTriTest(pBackData, uiBackStride, TriB, iScreenWidth, iScreenHeight);
				}
				
			}
			else if (state == RENDER_STATE::FLAT)
			{
				if (false == m_bWireframe)
				{
					FillTriSmooth(pBackData, uiBackStride, TriA, iScreenWidth, iScreenHeight);
					FillTriSmooth(pBackData, uiBackStride, TriB, iScreenWidth, iScreenHeight);
				}
				else
				{
					DrawTriSmooth(pBackData, uiBackStride, TriA, iScreenWidth, iScreenHeight);
					DrawTriSmooth(pBackData, uiBackStride, TriB, iScreenWidth, iScreenHeight);
				}
				
			}

		}

		pBackLock->Release();
	}
}
