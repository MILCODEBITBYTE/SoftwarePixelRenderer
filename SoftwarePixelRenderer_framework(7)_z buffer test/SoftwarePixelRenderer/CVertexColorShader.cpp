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

void CVertexColorShader::Render(CFace* pFace, CMaterial* pMaterial,  const RENDER_STATE state,  const bool bZTest)
{
	if (nullptr == m_pScreenBuffer) return;

	XMMATRIX matModel = XMLoadFloat4x4(&m_matModel);
	XMMATRIX matWorld = XMLoadFloat4x4(&m_matWorld);
	XMMATRIX matView = XMLoadFloat4x4(&m_matView);
	XMMATRIX matProj = XMLoadFloat4x4(&m_matProj);
	XMMATRIX matViewport = XMLoadFloat4x4(&m_matViewport);
	XMMATRIX matMW;
	XMMATRIX matVPV;

	matMW = XMMatrixMultiply(matModel, matWorld);
	matVPV = XMMatrixMultiply(matView, matProj);
	matVPV = XMMatrixMultiply(matVPV, matViewport);

	VERTEX* pVertices = pFace->GetVertices();
	size_t tVtxCnt = pFace->GetVertexCount();
	XMVECTOR vCalc;



	for(size_t i = 0; i <tVtxCnt; ++i)
	{
		vCalc = XMLoadFloat3(&pVertices[i].v3Vtx);
		vCalc = XMVector3TransformCoord(vCalc, matMW);
		vCalc = XMVector3TransformCoord(vCalc, matVPV);
		XMStoreFloat3(&pVertices[i].v3Vtx, vCalc);
	}

	if (tVtxCnt == 3)
		DrawTriangle(pVertices, nullptr,  pFace->GetMaterialName(),state, bZTest);
	else
		DrawQuad(pVertices,nullptr, pFace->GetMaterialName(), state, bZTest);
}


void CVertexColorShader::DrawTrilinearInterpolation(BYTE* pBack, const UINT stride, const VERTEX* pVertices[3], const bool bZTest)
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

	XMFLOAT4 v4Col;
	v4Col.x = (SortedCross[0]->v4Color.x + SortedCross[1]->v4Color.x + SortedCross[2]->v4Color.x) / 3.f;
	v4Col.y = (SortedCross[0]->v4Color.y + SortedCross[1]->v4Color.y + SortedCross[2]->v4Color.y) / 3.f;
	v4Col.z = (SortedCross[0]->v4Color.z + SortedCross[1]->v4Color.z + SortedCross[2]->v4Color.z) / 3.f;
	v4Col.w = (SortedCross[0]->v4Color.w + SortedCross[1]->v4Color.w + SortedCross[2]->v4Color.w) / 3.f;

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


	function<void(CBresenhamLine&, const WORD, const float , const float)> fcLineNextStep = [&](CBresenhamLine& line, const WORD num,  const float zStart, const float zEnd)
	{
		long X = 0;
		long Y = 0;
		float fZ=0;
		float fRate = 0;
		do
		{

			X = line.GetCurrentX();
			Y = line.GetCurrentY();

			LINE_DEPTH::LINE& dir = vecDepth[X - ptA.x].line[num];

			fRate = line.GetCurrentRate();
			if (line.isLeft())
				fRate = 1.f - fRate;

			dir.z = zStart * (1.f - fRate) + zEnd * fRate;

			dir.y = Y;

		} while (line.MoveNext());


	};

	fcLineNextStep(lineB, 2, SortedCross[0]->v3Vtx.z, SortedCross[2]->v3Vtx.z);
	fcLineNextStep(lineA, 0,  SortedCross[0]->v3Vtx.z, SortedCross[1]->v3Vtx.z);
	fcLineNextStep(lineAB, 1, SortedCross[1]->v3Vtx.z, SortedCross[2]->v3Vtx.z);

	float fRate;
	float fZStart, fZEnd;
	float fCurrZ = 1;

	long x;
	long lStartY, lEndY, lDirY;
	long lBackIndex;
	long lZIndex;

	long len = 0;
	long lCnt = 0;
	//draw top down line

	for (size_t step = 0; step < size; ++step)
	{
		x = long(ptA.x + step);

		if (x < 0 || x >= w) continue;

		//part A
		if (x < ptB.x)
		{
			lStartY = vecDepth[step].line[0].y;
			fZStart = vecDepth[step].line[0].z;
		}
		//part AB
		else
		{
			lStartY = vecDepth[step].line[1].y;
			fZStart = vecDepth[step].line[1].z;
		}
		//part B
		lEndY = vecDepth[step].line[2].y;
		fZEnd = vecDepth[step].line[2].z;

		if (lEndY < lStartY) lDirY = -1;
		else
			lDirY = 1;

		lEndY += lDirY;

		lCnt = 0;
		len = abs(lStartY - lEndY);
		for (long y = lStartY; y != lEndY; y += lDirY)
		{
			if (y < 0 || y >= h) continue;

			if (len == 0)
				fRate = 0;
			else
				fRate = lCnt / (float)len;

			lCnt++;

			if (bZTest)
			{
				fCurrZ = fZStart * (1.f - fRate) + fZEnd * fRate;

				lZIndex = y * w + x;


				if (false == ZTest(m_pZBuffer, lZIndex, fCurrZ))
					continue;
			}

			lBackIndex = y * stride + x * 4;

			pBack[lBackIndex + 0] = B;
			pBack[lBackIndex + 1] = G;
			pBack[lBackIndex + 2] = R;
			pBack[lBackIndex + 3] = 255;
		}
	}

}


void CVertexColorShader::DrawLinearInterpolation(BYTE* pBuffer, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, const bool bZTest)
{
	int w = int(m_matViewport._11 * 2.f);
	int h = int(m_matViewport._22 * -2.f);

	CPoint ptStart, ptEnd;

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

	float fLen = 0;
	float fZ1 = 1, fZ2 = 1;
	float fZStart = 1, fZEnd = 1;
	float fCurrZ = 1;

	fZStart = pStart->v3Vtx.z;
	fZEnd = pEnd->v3Vtx.z;

	XMFLOAT4 v4Col;
	v4Col.x = (pStart->v4Color.x + pEnd->v4Color.x) / 2.f;
	v4Col.y = (pStart->v4Color.y + pEnd->v4Color.y) / 2.f;
	v4Col.z = (pStart->v4Color.z + pEnd->v4Color.z) / 2.f;
	v4Col.w = (pStart->v4Color.w + pEnd->v4Color.w) / 2.f;


	BYTE R, G, B;
	R = BYTE(255 * v4Col.x);
	G = BYTE(255 * v4Col.y);
	B = BYTE(255 * v4Col.z);

	CBresenhamLine line;

	line.Initialize(ptStart, ptEnd);

	long lZIndex;
	long lBackIndex;
	float fRate;
	long x, y;

	do
	{
		x = line.GetCurrentX();
		y = line.GetCurrentY();

		fRate = line.GetCurrentRate();

		if (line.isLeft())
			fRate = 1.f - fRate;

		//ZBuffer test;
		if (true == bZTest)
		{
			fZ1 = fZStart * (1.f - fRate);
			fZ2 = fZEnd * fRate;

			fCurrZ = fZ1 + fZ2;

			lZIndex = y * w + x;

			if (false == ZTest(m_pZBuffer, lZIndex, fCurrZ))
				continue;
		}

		lBackIndex = y * uiStride + x * 4;

		pBuffer[lBackIndex + 0] = B;
		pBuffer[lBackIndex + 1] = G;
		pBuffer[lBackIndex + 2] = R;
		pBuffer[lBackIndex + 3] = 255;


	} while (line.MoveNext());

}






void CVertexColorShader::FillTrilinearInterpolation(BYTE* pBack, const UINT stride, const VERTEX* pVertices[3], const bool bZTest)
{
	int w = int(m_matViewport._11 * 2.f);
	int h = int(m_matViewport._22 * -2.f);

	BYTE R, G, B, A=255;

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

	XMFLOAT4 v4Col;

	typedef struct tagLineDepth
	{
		struct LINE
		{
			long y;
			float z;
			XMFLOAT4 col;
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

	function<void(CBresenhamLine&, const WORD, const XMFLOAT4, const XMFLOAT4, const float, const float)> fcLineNextStep = [&](CBresenhamLine& line, const WORD num, const XMFLOAT4 colStart, const XMFLOAT4 colEnd, const float zStart, const float zEnd)
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

			dir.col.x = colStart.x * (1.f - fRate) + colEnd.x * fRate;
			dir.col.y = colStart.y * (1.f - fRate) + colEnd.y * fRate;
			dir.col.z = colStart.z * (1.f - fRate) + colEnd.z * fRate;
			dir.col.w = colStart.w * (1.f - fRate) + colEnd.w * fRate;

			dir.y = Y;

		} while (line.MoveNext());


	};


	fcLineNextStep(lineB, 2, SortedCross[0]->v4Color, SortedCross[2]->v4Color, SortedCross[0]->v3Vtx.z, SortedCross[2]->v3Vtx.z);
	fcLineNextStep(lineA, 0, SortedCross[0]->v4Color, SortedCross[1]->v4Color, SortedCross[0]->v3Vtx.z, SortedCross[1]->v3Vtx.z);
	fcLineNextStep(lineAB,1, SortedCross[1]->v4Color, SortedCross[2]->v4Color, SortedCross[1]->v3Vtx.z, SortedCross[2]->v3Vtx.z);

	long x;
	long lCnt = 0;
	long len = 0;
	long lBackIndex;
	long lZIndex;
	long lStartY, lEndY, lDirY;
	float fCurrZ, fZStart, fZEnd;
	XMFLOAT4 colStart, colEnd;

	for (size_t step = 0; step < size; ++step)
	{
		x = long(ptA.x + step);

		if (x < 0 || x >= w) continue;

		//part A
		if (x < ptB.x)
		{
			lStartY = vecDepth[step].line[0].y;
			fZStart = vecDepth[step].line[0].z;
			colStart = vecDepth[step].line[0].col;
		}
		//part AB
		else
		{
			lStartY = vecDepth[step].line[1].y;
			fZStart = vecDepth[step].line[1].z;
			colStart = vecDepth[step].line[1].col;
		}

		//part B
		lEndY = vecDepth[step].line[2].y;
		fZEnd = vecDepth[step].line[2].z;
		colEnd = vecDepth[step].line[2].col;

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

			if (bZTest)
			{
				fCurrZ = fZStart * (1.f - fRate) + fZEnd * fRate;

				lZIndex = y * w + x;

				if (false == ZTest(m_pZBuffer, lZIndex, fCurrZ))
					continue;
			}


			v4Col.x = colStart.x * (1.f - fRate) + colEnd.x * fRate;
			v4Col.y = colStart.y * (1.f - fRate) + colEnd.y * fRate;
			v4Col.z = colStart.z * (1.f - fRate) + colEnd.z * fRate;
			v4Col.w = colStart.w * (1.f - fRate) + colEnd.w * fRate;

			R = BYTE(v4Col.x * 255.f);
			G = BYTE(v4Col.y * 255.f);
			B = BYTE(v4Col.z * 255.f);



			lBackIndex = y * stride + x * 4;

			pBack[lBackIndex + 0] = B;
			pBack[lBackIndex + 1] = G;
			pBack[lBackIndex + 2] = R;
			pBack[lBackIndex + 3] = 255;// A;
		}
	}

}


void CVertexColorShader::FillLinearInterpolation(BYTE* pBuffer, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, const bool bZTest)
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



	BYTE sR, sG, sB, sA, eR, eG, eB, eA, fR, fG, fB, fA;

	sR = BYTE(pStart->v4Color.x * 255.f);
	sG = BYTE(pStart->v4Color.y * 255.f);
	sB = BYTE(pStart->v4Color.z * 255.f);
	sA = BYTE(pStart->v4Color.w * 255.f);

	eR = BYTE(pEnd->v4Color.x * 255.f);
	eG = BYTE(pEnd->v4Color.y * 255.f);
	eB = BYTE(pEnd->v4Color.z * 255.f);
	eA = BYTE(pEnd->v4Color.w * 255.f);


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
	long lBackIndex;

	do
	{
		x = line.GetCurrentX();
		y = line.GetCurrentY();

		fRate = line.GetCurrentRate();
		if (line.isLeft())
			fRate = 1.f - fRate;

		//ZBuffer test;
		if (true == bZTest)
		{
			fZ1 = fZStart * (1.f - fRate);
			fZ2 = fZEnd * fRate;

			fCurrZ = fZ1 + fZ2;

			lZIndex = y * w + x;

			if (false == ZTest(m_pZBuffer, lZIndex, fCurrZ))
				continue;
		}

		fR = BYTE(sR * (1.f - fRate) + eR * fRate);
		fG = BYTE(sG * (1.f - fRate) + eG * fRate);
		fB = BYTE(sB * (1.f - fRate) + eB * fRate);
		fA = BYTE(sA * (1.f - fRate) + eA * fRate);

		lBackIndex = y * uiStride + x * 4;

		pBuffer[lBackIndex + 0] = fB;
		pBuffer[lBackIndex + 1] = fG;
		pBuffer[lBackIndex + 2] = fR;
		pBuffer[lBackIndex + 3] = fA;

	} while (line.MoveNext());


}




void CVertexColorShader::DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial,const wstring strMaterialName, const RENDER_STATE state, const bool bZTest)
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

			const VERTEX* Tri[3];
			Tri[0] = &pVertices[0];
			Tri[1] = &pVertices[1];
			Tri[2] = &pVertices[2];

			if (state == RENDER_STATE::BOUNDBOX)
			{
				DrawLinearInterpolation(pBackData, uiBackStride, pVertices, pVertices + 1, bZTest);
				DrawLinearInterpolation(pBackData, uiBackStride, pVertices + 1, pVertices + 2, bZTest);
				DrawLinearInterpolation(pBackData, uiBackStride, pVertices + 2, pVertices, bZTest);
			}

			else if (state == RENDER_STATE::TEST)
			{
				if (false == m_bWireframe)
				{
					DrawTrilinearInterpolation(pBackData, uiBackStride, Tri,  bZTest);
				}
				else
				{
					DrawLinearInterpolation(pBackData, uiBackStride, pVertices, pVertices + 1,  bZTest);
					DrawLinearInterpolation(pBackData, uiBackStride, pVertices + 1, pVertices + 2,  bZTest);
					DrawLinearInterpolation(pBackData, uiBackStride, pVertices + 2, pVertices, bZTest);
				}
			}
				
			else if (state == RENDER_STATE::FLAT)
			{
				if (false == m_bWireframe)
				{
					FillTrilinearInterpolation(pBackData, uiBackStride, Tri,  bZTest);
				}
				else
				{
					FillLinearInterpolation(pBackData, uiBackStride, pVertices, pVertices + 1,  bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, pVertices+1, pVertices + 2,  bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, pVertices+2, pVertices,  bZTest);
				}
			}
				
		}

		pBackLock->Release();
	}
}

void CVertexColorShader::DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const RENDER_STATE state, const bool bZTest)
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

			const VERTEX *TriA[3], *TriB[3];
			TriA[0] = &pVertices[0];
			TriA[1] = &pVertices[1];
			TriA[2] = &pVertices[3];

		

			TriB[0] = &pVertices[1];
			TriB[1] = &pVertices[2];
			TriB[2] = &pVertices[3];

			if (state == RENDER_STATE::BOUNDBOX)
			{
				DrawLinearInterpolation(pBackData, uiBackStride, TriA[0], TriA[1], bZTest);
				DrawLinearInterpolation(pBackData, uiBackStride, TriA[1], TriA[2], bZTest);
				DrawLinearInterpolation(pBackData, uiBackStride, TriA[2], TriA[0], bZTest);

				DrawLinearInterpolation(pBackData, uiBackStride, TriB[0], TriB[1], bZTest);
				DrawLinearInterpolation(pBackData, uiBackStride, TriB[1], TriB[2], bZTest);
				DrawLinearInterpolation(pBackData, uiBackStride, TriB[2], TriB[0], bZTest);
			}
			else if (state == RENDER_STATE::TEST)
			{
				if (false == m_bWireframe)
				{
					DrawTrilinearInterpolation(pBackData, uiBackStride, TriA, bZTest);
					DrawTrilinearInterpolation(pBackData, uiBackStride, TriB, bZTest);
				}
				else
				{

					DrawLinearInterpolation(pBackData, uiBackStride, TriA[0], TriA[1], bZTest);
					DrawLinearInterpolation(pBackData, uiBackStride, TriA[1], TriA[2], bZTest);
					DrawLinearInterpolation(pBackData, uiBackStride, TriA[2], TriA[0], bZTest);

					DrawLinearInterpolation(pBackData, uiBackStride, TriB[0], TriB[1], bZTest);
					DrawLinearInterpolation(pBackData, uiBackStride, TriB[1], TriB[2], bZTest);
					DrawLinearInterpolation(pBackData, uiBackStride, TriB[2], TriB[0], bZTest);
				}
				
			}
			else if (state == RENDER_STATE::FLAT)
			{
				if (false == m_bWireframe)
				{
					FillTrilinearInterpolation(pBackData, uiBackStride, TriA, bZTest);
					FillTrilinearInterpolation(pBackData, uiBackStride, TriB, bZTest);
				}
				else
				{
					FillLinearInterpolation(pBackData, uiBackStride, TriA[0], TriA[1], bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, TriA[1], TriA[2], bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, TriA[2], TriA[0], bZTest);

					FillLinearInterpolation(pBackData, uiBackStride, TriB[0], TriB[1], bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, TriB[1], TriB[2], bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, TriB[2], TriB[0], bZTest);

				}
				
			}

		}

		pBackLock->Release();
	}
}

