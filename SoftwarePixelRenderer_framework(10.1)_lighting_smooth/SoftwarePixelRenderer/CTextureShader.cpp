#include "pch.h"
#include "CTextureShader.h"
#include <functional>
#include <algorithm>

CTextureShader::CTextureShader()
{
}

CTextureShader::~CTextureShader()
{
}


void CTextureShader::Render(CFace* pFace, CMaterial* pMaterial, CLight *pLight,  const RENDER_STATE state, const bool bLight, const bool bZTest)
{
	if (nullptr == m_pScreenBuffer) return;

	XMMATRIX matModel = XMLoadFloat4x4(&m_matModel);
	XMMATRIX matWorld = XMLoadFloat4x4(&m_matWorld);
	XMMATRIX matView = XMLoadFloat4x4(&m_matView);
	XMMATRIX matProj = XMLoadFloat4x4(&m_matProj);
	XMMATRIX matViewport = XMLoadFloat4x4(&m_matViewport);
	XMMATRIX matMW;
	XMMATRIX matMWVP;
	XMMATRIX matMWVPV;

	matMW = XMMatrixMultiply(matModel, matWorld);
	matMWVP = XMMatrixMultiply(matMW, matView);
	matMWVP = XMMatrixMultiply(matMWVP, matProj);
	matMWVPV = XMMatrixMultiply(matMWVP, matViewport);

	VERTEX* pVertices = pFace->GetVertices();
	size_t tVtxCnt = pFace->GetVertexCount();
	XMVECTOR vCalc, v4Calc, vVW[3], vDir[2], vNormal;
	XMFLOAT4 v4Vtx;

	//CALC light
	
	if (bLight)
	{
		for (int i = 0; i < 3; ++i)
		{
			vVW[i] = XMLoadFloat3(&pVertices[i].v3Vtx);
			vVW[i] = XMVector3TransformCoord(vVW[i], matMW);
		}

		vDir[0] = XMVectorSubtract(vVW[0], vVW[1]);
		vDir[1] = XMVectorSubtract(vVW[2], vVW[1]);

		vNormal = XMVector3Normalize(XMVector3Cross(vDir[0], vDir[1]));

		for (int i = 0; i < 3; ++i)
			XMStoreFloat3(&pVertices[i].v3Normal, vNormal);

		if (pLight->GetType() == LIGHT_TYPE::SPOT)
			pLight->ComputeSpotlight(vVW[0], vNormal);
		else if (pLight->GetType() == LIGHT_TYPE::POINT)
			pLight->ComputePointLight(vVW[0], vNormal);
		else if (pLight->GetType() == LIGHT_TYPE::DIRECTIONAL)
			pLight->ComputeDirectionalLight(vNormal);
		else
			pLight->ComputePhong(vVW[0], vNormal);
	}
	
	//CALC UV
	float fRHW;

	for (size_t i = 0; i < tVtxCnt; ++i)
	{

		v4Calc = XMLoadFloat3(&pVertices[i].v3Vtx);
		v4Calc = XMVectorSetW(v4Calc, 1);
		v4Calc = XMVector4Transform(v4Calc, matMWVP);
		XMStoreFloat4(&v4Vtx, v4Calc);

		fRHW = 1.f/v4Vtx.w;

		pVertices[i].v3UV.x *= fRHW;
		pVertices[i].v3UV.y *= fRHW;
		pVertices[i].v3UV.z = fRHW;

		vCalc = XMLoadFloat3(&pVertices[i].v3Vtx);
		vCalc = XMVector3TransformCoord(vCalc, matMWVPV);
		XMStoreFloat3(&pVertices[i].v3Vtx, vCalc);
	}

	if (tVtxCnt == 3)
		DrawTriangle(pVertices, pMaterial, pLight, pFace->GetMaterialName(),state, bLight, bZTest);
	else
		DrawQuad(pVertices, pMaterial, pLight, pFace->GetMaterialName(),state, bLight, bZTest);
}

void CTextureShader::DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName,const RENDER_STATE state, const bool bLight, const bool bZTest)
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

			OBJ_MATERIAL_INFO* pInfo = pMaterial->GetMaterialInfo(strMaterialName);
			CWicBitmap* pSrc = pMaterial->GetTexture(pInfo->map_Kd);

			const VERTEX* Tri[3];
			Tri[0] = pVertices;
			Tri[1] = pVertices + 1;
			Tri[2] = pVertices + 2;

			if (false == m_bWireframe)
			{
				if (pSrc)
				{
					FillTrilinearInterpolation(pBackData, uiBackStride, Tri, pSrc, pLight, bLight, bZTest);
				}
			}
			else
			{
				if (pSrc)
				{
					FillLinearInterpolation(pBackData, uiBackStride, pVertices, pVertices + 1, pSrc, pLight, bLight, bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, pVertices + 1, pVertices + 2, pSrc, pLight, bLight, bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, pVertices + 2, pVertices, pSrc, pLight, bLight, bZTest);
				}

			}

		}

		pBackLock->Release();
	}


}

void CTextureShader::DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest)
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

			const VERTEX* TriA[3], * TriB[3];
			TriA[0] = &pVertices[0];
			TriA[1] = &pVertices[1];
			TriA[2] = &pVertices[3];



			TriB[0] = &pVertices[1];
			TriB[1] = &pVertices[2];
			TriB[2] = &pVertices[3];

			OBJ_MATERIAL_INFO* pInfo = pMaterial->GetMaterialInfo(strMaterialName);
			CWicBitmap* pSrc = pMaterial->GetTexture(pInfo->map_Kd);

			if (false == m_bWireframe)
			{
				if (pSrc)
				{
					FillTrilinearInterpolation(pBackData, uiBackStride, TriA, pSrc, pLight, bLight, bZTest);
					FillTrilinearInterpolation(pBackData, uiBackStride, TriB, pSrc, pLight, bLight, bZTest);
				}
			}
			else
			{
				if (pSrc)
				{
					FillLinearInterpolation(pBackData, uiBackStride, TriA[0], TriA[1], pSrc, pLight, bLight, bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, TriA[1], TriA[2], pSrc, pLight, bLight, bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, TriA[2], TriA[0], pSrc, pLight, bLight, bZTest);

					FillLinearInterpolation(pBackData, uiBackStride, TriB[0], TriB[1], pSrc, pLight, bLight, bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, TriB[1], TriB[2], pSrc, pLight, bLight, bZTest);
					FillLinearInterpolation(pBackData, uiBackStride, TriB[2], TriB[0], pSrc, pLight, bLight, bZTest);
				}
			}
		}

		pBackLock->Release();
	}
}

void CTextureShader::FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CWicBitmap* pTex, CLight* pLight, const bool bLight, const bool bZTest)
{
	if (nullptr == pTex) return;

	int w = int(m_matViewport._11 * 2.f);
	int h = int(m_matViewport._22 * -2.f);
	int half_w = w / 2;
	int half_h = h / 2;

	HRESULT hr;
	IWICBitmap* pSrc = pTex->GetWicBitmap();
	UINT uiSrcWidth, uiSrcHeight;
	pSrc->GetSize(&uiSrcWidth, &uiSrcHeight);

	WICRect rcSrc;
	rcSrc.X = 0;
	rcSrc.Y = 0;
	rcSrc.Width = uiSrcWidth;
	rcSrc.Height = uiSrcHeight;

	IWICBitmapLock* pSrcLock;
	hr = pSrc->Lock(&rcSrc, WICBitmapLockRead, &pSrcLock);
	if (FAILED(hr)) return;

	BYTE* pSrcData;
	UINT uiSrcSize;
	UINT uiSrcStride;
	hr = pSrcLock->GetDataPointer(&uiSrcSize, &pSrcData);
	if (FAILED(hr)) return;

	pSrcLock->GetStride(&uiSrcStride);

	//텍스쳐에 대한 삼선형 보간 구조체
	typedef struct _tagTrilinear
	{
		struct INTERPOLATION
		{
			long y;
			float z;
			XMFLOAT3 UV;
		};

		INTERPOLATION interpolation[3];

	}TRILINEAR;

	vector<TRILINEAR> vecTrilinear;


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

	vecTrilinear.resize(abs(ptA.x - ptC.x) + 1);
	size_t size = vecTrilinear.size();

	//3선형 보간
	function<void(CBresenhamLine&, const WORD, const XMFLOAT3&, const XMFLOAT3&, const float, const float)> fcLineNextStep = [&](CBresenhamLine& line, const WORD num, const XMFLOAT3& v3StartUV, const XMFLOAT3& v3EndUV, const float fStartZ, const float fEndZ)
	{
		long X = 0;
		long Y = 0;
		float fRate = 0;

		do
		{

			X = line.GetCurrentX();
			Y = line.GetCurrentY();

			fRate = line.GetCurrentRate();

			if (line.isLeft())
				fRate = 1.f - fRate;

			TRILINEAR::INTERPOLATION& dir = vecTrilinear[abs(X - ptA.x)].interpolation[num];


			dir.UV.x = v3StartUV.x * (1.f - fRate) + v3EndUV.x * fRate;
			dir.UV.y = v3StartUV.y * (1.f - fRate) + v3EndUV.y * fRate;
			dir.UV.z = v3StartUV.z * (1.f - fRate) + v3EndUV.z * fRate;

			dir.z = fStartZ * (1.f - fRate) + fEndZ * fRate;

			dir.y = Y;

		} while (line.MoveNext());
	};

	fcLineNextStep(lineB, 2, SortedCross[0]->v3UV, SortedCross[2]->v3UV, SortedCross[0]->v3Vtx.z, SortedCross[2]->v3Vtx.z);
	fcLineNextStep(lineA, 0, SortedCross[0]->v3UV, SortedCross[1]->v3UV, SortedCross[0]->v3Vtx.z, SortedCross[1]->v3Vtx.z);
	fcLineNextStep(lineAB, 1, SortedCross[1]->v3UV, SortedCross[2]->v3UV, SortedCross[1]->v3Vtx.z, SortedCross[2]->v3Vtx.z);

	long x;
	long lCnt = 0;
	long len = 0;
	long lStartY, lEndY, lDirY;
	long lZIndex = 0;
	float fZStart, fZEnd;
	float fCurrZ;
	float U, V;

	XMFLOAT4 v4ColFinal;
	XMFLOAT3 v3StartUV, v3EndUV;
	RGBQUAD colFinal;

	float fRate;

	VERTEX vtxFinal;
	vtxFinal.v3Normal = SortedCross[0]->v3Normal;

	UINT uiFinalU, uiFinalV;
	long lBackIndex = 0;
	long lSrcIndex = 0;

	//세이더 파이프라인
	for (size_t step = 0; step < size; ++step)
	{
		x = long(ptA.x + step);

		if (x < 0 || x >= w) continue;

		//part A
		if (x < ptB.x)
		{
			lStartY = vecTrilinear[step].interpolation[0].y;
			v3StartUV = vecTrilinear[step].interpolation[0].UV;
			fZStart = vecTrilinear[step].interpolation[0].z;
		}
		//part AB
		else
		{
			lStartY = vecTrilinear[step].interpolation[1].y;
			v3StartUV = vecTrilinear[step].interpolation[1].UV;
			fZStart = vecTrilinear[step].interpolation[1].z;
		}

		//part B
		lEndY = vecTrilinear[step].interpolation[2].y;
		v3EndUV = vecTrilinear[step].interpolation[2].UV;
		fZEnd = vecTrilinear[step].interpolation[2].z;

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

			//3선형 보간 완료
			vtxFinal.v3Vtx.x = (float)x;
			vtxFinal.v3Vtx.y = (float)y;
			vtxFinal.v3Vtx.z = fCurrZ;

			lZIndex = y * w + x;

			if (bZTest)
				if (false == ZTest(m_pZBuffer, lZIndex, fCurrZ))
					continue;

			vtxFinal.v3UV.x = v3StartUV.x * (1.f - fRate) + v3EndUV.x * fRate;
			vtxFinal.v3UV.y = v3StartUV.y * (1.f - fRate) + v3EndUV.y * fRate;
			vtxFinal.v3UV.z = v3StartUV.z * (1.f - fRate) + v3EndUV.z * fRate;

			U = vtxFinal.v3UV.x / vtxFinal.v3UV.z * (float)uiSrcWidth;
			V = vtxFinal.v3UV.y / vtxFinal.v3UV.z * (float)uiSrcHeight;


			uiFinalU = (UINT)floorf(U);
			uiFinalV = (UINT)floorf(V);

			if (uiFinalU >= uiSrcWidth)
				uiFinalU = uiSrcWidth - 1;

			if (uiFinalV >= uiSrcHeight)
				uiFinalV = uiSrcHeight - 1;

			lSrcIndex = (uiSrcHeight - 1 - uiFinalV) * uiSrcStride + uiFinalU * pTex->GetChannel();

			colFinal.rgbRed = pSrcData[lSrcIndex + 2];
			colFinal.rgbGreen = pSrcData[lSrcIndex + 1];
			colFinal.rgbBlue = pSrcData[lSrcIndex + 0];
			colFinal.rgbReserved = 255;// pSrc[lSrcIndex + 3];

			if (bLight)
			{
				v4ColFinal = pLight->GetComputedColor();

				colFinal.rgbRed = BYTE(colFinal.rgbRed * v4ColFinal.x);
				colFinal.rgbGreen = BYTE(colFinal.rgbGreen * v4ColFinal.y);
				colFinal.rgbBlue = BYTE(colFinal.rgbBlue * v4ColFinal.z);
				colFinal.rgbReserved = 255;// BYTE(colFinal.rgbReserved * v4ColFinal.w);

			}

			lBackIndex = y * uiStride + x * 4;

			pBack[lBackIndex + 0] = colFinal.rgbBlue;
			pBack[lBackIndex + 1] = colFinal.rgbGreen;
			pBack[lBackIndex + 2] = colFinal.rgbRed;
			pBack[lBackIndex + 3] = colFinal.rgbReserved;

		}
	}

	pSrcLock->Release();
}

void CTextureShader::FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CWicBitmap* pTex, CLight* pLight, const bool bLight, const bool bZTest)
{
	int w = int(m_matViewport._11 * 2.f);
	int h = int(m_matViewport._22 * -2.f);
	int half_w = w / 2;
	int half_h = h / 2;

	HRESULT hr;
	IWICBitmap* pSrc = pTex->GetWicBitmap();
	UINT uiSrcWidth, uiSrcHeight;
	pSrc->GetSize(&uiSrcWidth, &uiSrcHeight);

	WICRect rcSrc;
	rcSrc.X = 0;
	rcSrc.Y = 0;
	rcSrc.Width = uiSrcWidth;
	rcSrc.Height = uiSrcHeight;

	IWICBitmapLock* pSrcLock;
	hr = pSrc->Lock(&rcSrc, WICBitmapLockRead, &pSrcLock);
	if (FAILED(hr)) return;

	BYTE* pSrcData;
	UINT uiSrcSize;
	UINT uiSrcStride;
	hr = pSrcLock->GetDataPointer(&uiSrcSize, &pSrcData);
	if (FAILED(hr)) return;

	pSrcLock->GetStride(&uiSrcStride);

	CPoint ptA = CPoint(int(roundf(pStart->v3Vtx.x)), int(roundf(pStart->v3Vtx.y)));
	CPoint ptB = CPoint(int(roundf(pEnd->v3Vtx.x)), int(roundf(pEnd->v3Vtx.y)));

	CBresenhamLine line;
	line.Initialize(ptA, ptB);

	long X = 0;
	long Y = 0;
	float fRate = 0;
	float U, V;

	long lZIndex = 0;
	
	RGBQUAD colFinal;

	XMFLOAT4 v4ColFinal;
	VERTEX vtxFinal;
	vtxFinal.v3Normal = pStart->v3Normal;

	UINT uiFinalU, uiFinalV;

	long lBackIndex = 0;
	long lSrcIndex = 0;

	//선형 보간
	do
	{

		X = line.GetCurrentX();
		Y = line.GetCurrentY();

		fRate = line.GetCurrentRate();

		if (line.isLeft())
			fRate = 1.f - fRate;

		vtxFinal.v3UV.x = pStart->v3UV.x * (1.f - fRate) + pEnd->v3UV.x * fRate;
		vtxFinal.v3UV.y = pStart->v3UV.y * (1.f - fRate) + pEnd->v3UV.y * fRate;
		vtxFinal.v3UV.z = pStart->v3UV.z * (1.f - fRate) + pEnd->v3UV.z * fRate;

		vtxFinal.v3Vtx.x = (float)X;
		vtxFinal.v3Vtx.y = (float)Y;
		vtxFinal.v3Vtx.z = pStart->v3Vtx.z * (1.f - fRate) + pEnd->v3Vtx.z * fRate;

		if (X >= w) X = w - 1;
		if (Y >= h) Y = h - 1;

		//z test
		lZIndex = Y * w + X;

		if (bZTest)
			if (false == ZTest(m_pZBuffer, lZIndex, vtxFinal.v3Vtx.z))
				continue;

		U = vtxFinal.v3UV.x / vtxFinal.v3UV.z * (float)uiSrcWidth;
		V = vtxFinal.v3UV.y / vtxFinal.v3UV.z * (float)uiSrcHeight;


		uiFinalU = (UINT)floorf(U);
		uiFinalV = (UINT)floorf(V);

		if (uiFinalU >= uiSrcWidth)
			uiFinalU = uiSrcWidth - 1;

		if (uiFinalV >= uiSrcHeight)
			uiFinalV = uiSrcHeight - 1;

		lSrcIndex = (uiSrcHeight - 1 - uiFinalV) * uiSrcStride + uiFinalU * pTex->GetChannel();

		colFinal.rgbRed = pSrcData[lSrcIndex + 2];
		colFinal.rgbGreen = pSrcData[lSrcIndex + 1];
		colFinal.rgbBlue = pSrcData[lSrcIndex + 0];
		colFinal.rgbReserved = 255;// pSrc[lSrcIndex + 3];

		if (bLight)
		{
			v4ColFinal = pLight->GetComputedColor();

			colFinal.rgbRed = BYTE(colFinal.rgbRed * v4ColFinal.x);
			colFinal.rgbGreen = BYTE(colFinal.rgbGreen * v4ColFinal.y);
			colFinal.rgbBlue = BYTE(colFinal.rgbBlue * v4ColFinal.z);
			colFinal.rgbReserved = 255;// BYTE(colFinal.rgbReserved * v4ColFinal.w);

		}

		lBackIndex = Y * uiStride + X * 4;

		pBack[lBackIndex + 0] = colFinal.rgbBlue;
		pBack[lBackIndex + 1] = colFinal.rgbGreen;
		pBack[lBackIndex + 2] = colFinal.rgbRed;
		pBack[lBackIndex + 3] = colFinal.rgbReserved;

	} while (line.MoveNext());

	pSrcLock->Release();
}
