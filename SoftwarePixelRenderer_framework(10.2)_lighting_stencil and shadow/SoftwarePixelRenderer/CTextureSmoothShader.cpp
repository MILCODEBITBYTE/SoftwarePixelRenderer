#include "pch.h"
#include "CTextureSmoothShader.h"
#include <functional>
#include <algorithm>

CTextureSmoothShader::CTextureSmoothShader()
{
}

CTextureSmoothShader::~CTextureSmoothShader()
{
}

void CTextureSmoothShader::Render(CFace* pFace, CMaterial* pMaterial, CLight* pLight, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil)
{
	if (nullptr == m_pScreenBuffer) return;

	
	if (pFace->GetVertexCount() == 3)
		DrawTriangle(pFace->GetVertices(), pMaterial, pLight, pFace->GetMaterialName(),state, bLight, bZTest, bStencil);
	else
		DrawQuad(pFace->GetVertices(), pMaterial, pLight, pFace->GetMaterialName(), state, bLight, bZTest, bStencil);
}

void CTextureSmoothShader::DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil)
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
					FillTrilinearInterpolation(pBackData, uiBackStride, Tri, pSrc, pLight, bLight, bZTest, bStencil);
				}
			}
			else
			{
				if (pSrc)
				{
					FillLinearInterpolation(pBackData, uiBackStride, pVertices, pVertices + 1, pSrc, pLight, bLight, bZTest, bStencil);
					FillLinearInterpolation(pBackData, uiBackStride, pVertices + 1, pVertices + 2, pSrc, pLight, bLight, bZTest, bStencil);
					FillLinearInterpolation(pBackData, uiBackStride, pVertices + 2, pVertices, pSrc, pLight, bLight, bZTest, bStencil);
				}

			}

		}

		pBackLock->Release();
	}

}

void CTextureSmoothShader::DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil)
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

			const VERTEX *TriA[3], *TriB[3];
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
					FillTrilinearInterpolation(pBackData, uiBackStride, TriA, pSrc, pLight, bLight, bZTest, bStencil);
					FillTrilinearInterpolation(pBackData, uiBackStride, TriB, pSrc, pLight, bLight, bZTest, bStencil);
				}
			}
			else
			{
				if (pSrc)
				{
					FillLinearInterpolation(pBackData, uiBackStride, TriA[0], TriA[1], pSrc, pLight, bLight, bZTest, bStencil);
					FillLinearInterpolation(pBackData, uiBackStride, TriA[1], TriA[2], pSrc, pLight, bLight, bZTest, bStencil);
					FillLinearInterpolation(pBackData, uiBackStride, TriA[2], TriA[0], pSrc, pLight,bLight, bZTest, bStencil);

					FillLinearInterpolation(pBackData, uiBackStride, TriB[0], TriB[1], pSrc, pLight,bLight, bZTest, bStencil);
					FillLinearInterpolation(pBackData, uiBackStride, TriB[1], TriB[2], pSrc, pLight, bLight, bZTest, bStencil);
					FillLinearInterpolation(pBackData, uiBackStride, TriB[2], TriB[0], pSrc, pLight, bLight, bZTest, bStencil);
				}
			}
		}

		pBackLock->Release();
	}
	
}

void CTextureSmoothShader::FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CWicBitmap* pTex, CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil)
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
			XMFLOAT4 v4LerpUVZ;
		};

		INTERPOLATION interpolation[3];

	}TRILINEAR;

	vector<TRILINEAR> vecTrilinear;



	XMMATRIX matModel = XMLoadFloat4x4(&m_matModel);
	XMMATRIX matWorld = XMLoadFloat4x4(&m_matWorld);
	XMMATRIX matView = XMLoadFloat4x4(&m_matView);
	XMMATRIX matProj = XMLoadFloat4x4(&m_matProj);
	XMMATRIX matViewport = XMLoadFloat4x4(&m_matViewport);
	XMVECTOR vVer, vDet;
	VERTEX vtxTarget[3];

	m_matToProj = XMMatrixMultiply(matModel, matWorld);
	m_matToProj = XMMatrixMultiply(m_matToProj, matView);
	m_matToProj = XMMatrixMultiply(m_matToProj, matProj);
	m_matToViewport = XMMatrixMultiply(m_matToProj, matViewport);

	//버텍스, 픽셀 세이더에서 다시 계산하기위한 역 모델공간 행렬
	vDet = XMMatrixDeterminant(m_matToViewport);
	m_matInvToModel = XMMatrixInverse(&vDet, m_matToViewport);

	//Bresenahm 알고리즘을 적용하기위한 Viewport 변환
	for (int i = 0; i < 3; ++i)
	{
		vtxTarget[i] = *pVertices[i];

		vVer = XMLoadFloat3(&vtxTarget[i].v3Vtx);
		vVer = XMVector3TransformCoord(vVer, m_matToViewport);
		XMStoreFloat3(&vtxTarget[i].v3Vtx, vVer);
	}

	const VERTEX* SortedCross[3];
	SortedCross[0] = &vtxTarget[0];
	SortedCross[1] = &vtxTarget[1];
	SortedCross[2] = &vtxTarget[2];

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


	XMVECTOR vStartUVZ, vEndUVZ, vFinalUVZ;

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

			vStartUVZ = XMVectorSet(v3StartUV.x, v3StartUV.y, v3StartUV.z, fStartZ);
			vEndUVZ = XMVectorSet(v3EndUV.x, v3EndUV.y, v3EndUV.z, fEndZ);

			vFinalUVZ = XMVectorLerpV(vStartUVZ, vEndUVZ, XMVectorReplicate(fRate));
			XMStoreFloat4(&dir.v4LerpUVZ, vFinalUVZ);


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
	float fCurrZ;

	XMFLOAT4 v4StartUVZ, v4EndUVZ, v4FinalUVZ;
	RGBQUAD colFinal;
	float fRate;

	VERTEX vtxFinal;
	vtxFinal.v3Normal = SortedCross[0]->v3Normal;

	long lBackIndex = 0;
	long lStencilIndex=0;

	bool IsNotShadow = false;

	VERTEX_INPUT_TYPE v_in;
	PIXEL_INPUT_TYPE p_in;

	//세이더 파이프라인
	for (size_t step = 0; step < size; ++step)
	{
		x = long(ptA.x + step);

		if (x < 0 || x >= w) continue;

		//part A
		if (x < ptB.x)
		{
			lStartY = vecTrilinear[step].interpolation[0].y;
			v4StartUVZ = vecTrilinear[step].interpolation[0].v4LerpUVZ;
		}
		//part AB
		else
		{
			lStartY = vecTrilinear[step].interpolation[1].y;
			v4StartUVZ = vecTrilinear[step].interpolation[1].v4LerpUVZ;
		}

		//part B
		lEndY = vecTrilinear[step].interpolation[2].y;
		v4EndUVZ = vecTrilinear[step].interpolation[2].v4LerpUVZ;

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

			vStartUVZ = XMLoadFloat4(&v4StartUVZ);
			vEndUVZ = XMLoadFloat4(&v4EndUVZ);
			vFinalUVZ = XMVectorLerpV(vStartUVZ, vEndUVZ, XMVectorReplicate(fRate));

			XMStoreFloat4(&v4FinalUVZ, vFinalUVZ);

			fCurrZ = v4FinalUVZ.w;

			vtxFinal.v3UV.x = v4FinalUVZ.x;
			vtxFinal.v3UV.y = v4FinalUVZ.y;
			vtxFinal.v3UV.z = v4FinalUVZ.z;

			//3선형 보간 완료
			vtxFinal.v3Vtx.x = (float)x;
			vtxFinal.v3Vtx.y = (float)y;
			vtxFinal.v3Vtx.z = fCurrZ;

			lZIndex = y * w + x;

			if (bZTest)
				if (false == ZTest(m_pZBuffer, lZIndex, fCurrZ))
					continue;



			//세이딩을 위한 모델공간 역변환
			vVer = XMLoadFloat3(&vtxFinal.v3Vtx);
			vVer = XMVector3TransformCoord(vVer, m_matInvToModel);
			XMStoreFloat3(&vtxFinal.v3Vtx, vVer);

			v_in.vtx = vtxFinal;
			v_in.matLightView = pLight->GetLightViewMatrix();
			v_in.v3LightPos = pLight->GetPosition();

			//버텍스 세이딩
			p_in = VertexShading(v_in, pLight, bLight);

			
			//픽셀 세이딩

			colFinal = PixelShading(pSrcData, uiSrcStride, uiSrcWidth, uiSrcHeight, pTex->GetChannel(), p_in, pLight, bLight);

			//기타...
			if (bStencil)
			{
				UINT uiDepthX, uiDepthY;
				uiDepthX = (UINT)p_in.v3LightViewPos.x;
				uiDepthY = (UINT)p_in.v3LightViewPos.y;

				if ((uiDepthX >= 0 && uiDepthX < w) &&
					(uiDepthY >= 0 && uiDepthY < h))
				{
					lStencilIndex = uiDepthY * w + uiDepthX;

					//Stencil 테스트
					if (false == ZTest(m_pStencilBuffer, lStencilIndex, p_in.v3LightViewPos.z, false))
					{
						XMFLOAT4 v4FinalAmbient = pLight->GetAmbientColor();


						colFinal.rgbRed = BYTE(colFinal.rgbRed * v4FinalAmbient.x);
						colFinal.rgbGreen = BYTE(colFinal.rgbGreen * v4FinalAmbient.y);
						colFinal.rgbBlue = BYTE(colFinal.rgbBlue * v4FinalAmbient.z);
					}
				}
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

void CTextureSmoothShader::FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CWicBitmap* pTex, CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil)
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

	XMMATRIX matModel = XMLoadFloat4x4(&m_matModel);
	XMMATRIX matWorld = XMLoadFloat4x4(&m_matWorld);
	XMMATRIX matView = XMLoadFloat4x4(&m_matView);
	XMMATRIX matProj = XMLoadFloat4x4(&m_matProj);
	XMMATRIX matViewport = XMLoadFloat4x4(&m_matViewport);
	XMVECTOR vVer, vDet;
	VERTEX vtxStart, vtxEnd;

	m_matToProj = XMMatrixMultiply(matModel, matWorld);
	m_matToProj = XMMatrixMultiply(m_matToProj, matView);
	m_matToProj = XMMatrixMultiply(m_matToProj, matProj);
	m_matToViewport = XMMatrixMultiply(m_matToProj, matViewport);

	//버텍스, 픽셀 세이더에서 다시 계산하기위한 역 모델공간 행렬
	vDet = XMMatrixDeterminant(m_matToViewport);
	m_matInvToModel = XMMatrixInverse(&vDet, m_matToViewport);

	//Bresenahm 알고리즘을 적용하기위한 Viewport 변환

	vtxStart = *pStart;
	vtxEnd = *pEnd;

	vVer = XMLoadFloat3(&vtxStart.v3Vtx);
	vVer = XMVector3TransformCoord(vVer, m_matToViewport);
	XMStoreFloat3(&vtxStart.v3Vtx, vVer);

	vVer = XMLoadFloat3(&vtxEnd.v3Vtx);
	vVer = XMVector3TransformCoord(vVer, m_matToViewport);
	XMStoreFloat3(&vtxEnd.v3Vtx, vVer);

	CPoint ptA = CPoint(int(roundf(vtxStart.v3Vtx.x)), int(roundf(vtxStart.v3Vtx.y)));
	CPoint ptB = CPoint(int(roundf(vtxEnd.v3Vtx.x)), int(roundf(vtxEnd.v3Vtx.y)));

	CBresenhamLine line;
	line.Initialize(ptA, ptB);

	long X = 0;
	long Y = 0;
	float fRate = 0;
	float fCurrZ;

	long lZIndex = 0;

	RGBQUAD colFinal;

	VERTEX vtxFinal;
	vtxFinal.v3Normal = vtxStart.v3Normal;

	long lBackIndex = 0;
	long lStencilIndex = 0;

	XMVECTOR vStartUVZ, vEndUVZ, vFinalUVZ;

	VERTEX_INPUT_TYPE v_in;
	PIXEL_INPUT_TYPE p_in;

	XMVECTOR test;

	//선형 보간
	do
	{

		X = line.GetCurrentX();
		Y = line.GetCurrentY();

		fRate = line.GetCurrentRate();

		if (line.isLeft())
			fRate = 1.f - fRate;


		vStartUVZ = XMVectorSet(vtxStart.v3UV.x, vtxStart.v3UV.y, vtxStart.v3UV.z, vtxStart.v3Vtx.z);
		vEndUVZ = XMVectorSet(vtxEnd.v3UV.x, vtxEnd.v3UV.y, vtxEnd.v3UV.z, vtxEnd.v3Vtx.z);

		vFinalUVZ = XMVectorLerpV(vStartUVZ, vEndUVZ, XMVectorReplicate(fRate));
		
		XMStoreFloat3(&vtxFinal.v3UV, vFinalUVZ);


		vtxFinal.v3Vtx.x = (float)X;
		vtxFinal.v3Vtx.y = (float)Y;
		vtxFinal.v3Vtx.z = XMVectorGetW(vFinalUVZ);
		fCurrZ = vtxFinal.v3Vtx.z;

		if (X >= w) X = w - 1;
		if (Y >= h) Y = h - 1;

		//z test
		lZIndex = Y * w + X;

		if (bZTest)
			if (false == ZTest(m_pZBuffer, lZIndex, fCurrZ))
				continue;

		//세이더 파이프라인

		//세이딩을 위한 모델공간 역변환
		vVer = XMLoadFloat3(&vtxFinal.v3Vtx);
		vVer = XMVector3TransformCoord(vVer, m_matInvToModel);
		XMStoreFloat3(&vtxFinal.v3Vtx, vVer);
		test = XMVector3TransformCoord(vVer, m_matToViewport);

		v_in.vtx = vtxFinal;
		v_in.matLightView = pLight->GetLightViewMatrix();
		v_in.v3LightPos = pLight->GetPosition();

		//버텍스 세이딩
		p_in = VertexShading(v_in, pLight, bLight);


		//픽셀 세이딩

		colFinal = PixelShading(pSrcData, uiSrcStride, uiSrcWidth, uiSrcHeight, pTex->GetChannel(), p_in, pLight, bLight);

		//기타...
		if (bStencil)
		{
			UINT uiDepthX, uiDepthY;
			uiDepthX = (UINT)p_in.v3LightViewPos.x;
			uiDepthY = (UINT)p_in.v3LightViewPos.y;

			if ((uiDepthX >= 0 && uiDepthX < w) &&
				(uiDepthY >= 0 && uiDepthY < h))
			{
				lStencilIndex = uiDepthY * w + uiDepthX;
				//Stencil 테스트
				if (false == ZTest(m_pStencilBuffer, lStencilIndex, p_in.v3LightViewPos.z, false))
				{
					XMFLOAT4 v4FinalAmbient = pLight->GetAmbientColor();


					colFinal.rgbRed = BYTE(colFinal.rgbRed * v4FinalAmbient.x);
					colFinal.rgbGreen = BYTE(colFinal.rgbGreen * v4FinalAmbient.y);
					colFinal.rgbBlue = BYTE(colFinal.rgbBlue * v4FinalAmbient.z);
				}

			}
		}

		lBackIndex = Y * uiStride + X * 4;

		pBack[lBackIndex + 0] = colFinal.rgbBlue;
		pBack[lBackIndex + 1] = colFinal.rgbGreen;
		pBack[lBackIndex + 2] = colFinal.rgbRed;
		pBack[lBackIndex + 3] = colFinal.rgbReserved;

	} while (line.MoveNext());

	pSrcLock->Release();
}

PIXEL_INPUT_TYPE CTextureSmoothShader::VertexShading(const VERTEX_INPUT_TYPE& _in, CLight* pLight, const bool bLight)
{
	XMMATRIX matModel = XMLoadFloat4x4(&m_matModel);
	XMMATRIX matWorld = XMLoadFloat4x4(&m_matWorld);
	XMMATRIX matProj = XMLoadFloat4x4(&m_matProj);
	XMMATRIX matViewport = XMLoadFloat4x4(&m_matViewport);
	XMMATRIX matMW;
	matMW = XMMatrixMultiply(matModel, matWorld);



	PIXEL_INPUT_TYPE out;
	out.vtx = _in.vtx;

	XMVECTOR v4Calc, vWorld, vNormal;
	XMFLOAT4 v4Vtx;


	//CALC LIGHT
	if (bLight)
	{
		vWorld = XMLoadFloat3(&_in.vtx.v3Vtx);
		vWorld = XMVector3TransformCoord(vWorld, matMW);
		vNormal = XMLoadFloat3(&_in.vtx.v3Normal);
		vNormal = XMVector3Normalize(XMVector3TransformCoord(vNormal, matMW));
		XMStoreFloat3(&out.vtx.v3Normal, vNormal);

		LIGHT_TYPE type = pLight->GetType();

		if (type == LIGHT_TYPE::DIRECTIONAL)
		{
			XMFLOAT3 v3Pos = pLight->GetPosition();
			XMVECTOR vDir = XMVector3Normalize(XMLoadFloat3(&v3Pos));
			float fIntensity = XMVectorGetX(XMVector3Dot(vNormal, XMVectorNegate(vDir)));
			pLight->SetIntensity(fIntensity);
		}
		else if (type == LIGHT_TYPE::SPOT)
		{
			pLight->SetSpotlightDirection(vWorld, vNormal);
		}
		else if (type == LIGHT_TYPE::PHONG)
		{
			pLight->SetPhongDirection(vWorld, vNormal);
		}
		else
		{
			pLight->SetPointLightDirection(vWorld, vNormal);
		}


	}


	//CALC UV
	float fRHW;

	v4Calc = XMLoadFloat3(&_in.vtx.v3Vtx);
	v4Calc = XMVectorSetW(v4Calc, 1);
	v4Calc = XMVector4Transform(v4Calc, m_matToProj);
	XMStoreFloat4(&v4Vtx, v4Calc);

	v4Calc = XMLoadFloat3(&_in.vtx.v3Vtx);
	v4Calc = XMVector3TransformCoord(v4Calc, m_matToViewport);
	XMStoreFloat3(&out.vtx.v3Vtx, v4Calc);

	fRHW = 1.f / v4Vtx.w;

	XMVECTOR vRHW = XMVectorReplicate(v4Vtx.w);


	XMVECTOR vUV = XMVectorSet(out.vtx.v3UV.x, out.vtx.v3UV.y, out.vtx.v3UV.z, 0);
	vUV = XMVectorDivide(vUV, vRHW);
	XMStoreFloat3(&out.vtx.v3UV, vUV);
	out.vtx.v3UV.z = fRHW;



	//CALC light view pos
	XMMATRIX matLightWVPV;
	v4Calc = XMLoadFloat3(&_in.vtx.v3Vtx);
	v4Calc = XMVector3TransformCoord(v4Calc, matMW);
	v4Calc = XMVector3TransformCoord(v4Calc, _in.matLightView);
	v4Calc = XMVector3TransformCoord(v4Calc, matProj);
	v4Calc = XMVector3TransformCoord(v4Calc, matViewport);

	XMStoreFloat3(&out.v3LightViewPos, v4Calc);

	return out;
}

RGBQUAD CTextureSmoothShader::PixelShading(BYTE* pSrc, const UINT uiSrcStride, const UINT uiSrcWidth, const UINT uiSrcHeight, const int iChannel, const PIXEL_INPUT_TYPE& _in, CLight* pLight, const bool bLight)
{
	RGBQUAD colFinal;
	XMFLOAT4 v4Final;
	float U, V;
	UINT uiFinalU, uiFinalV;

	long lSrcIndex;

	U = _in.vtx.v3UV.x / _in.vtx.v3UV.z * (float)uiSrcWidth;
	V = _in.vtx.v3UV.y / _in.vtx.v3UV.z * (float)uiSrcHeight;


	uiFinalU = (UINT)floorf(U);
	uiFinalV = (UINT)floorf(V);

	if (uiFinalU >= uiSrcWidth)
		uiFinalU = uiSrcWidth - 1;

	if (uiFinalV >= uiSrcHeight)
		uiFinalV = uiSrcHeight - 1;

	lSrcIndex = (uiSrcHeight - 1 - uiFinalV) * uiSrcStride + uiFinalU * iChannel;

	colFinal.rgbRed = pSrc[lSrcIndex + 2];
	colFinal.rgbGreen = pSrc[lSrcIndex + 1];
	colFinal.rgbBlue = pSrc[lSrcIndex + 0];
	colFinal.rgbReserved = 255;// pSrc[lSrcIndex + 3];



	if (bLight)
	{
		XMFLOAT4 v4Diffuse = XMFLOAT4(colFinal.rgbRed / 255.f, colFinal.rgbGreen / 255.f, colFinal.rgbBlue / 255.f, 1);
		LIGHT_TYPE type = pLight->GetType();

		XMVECTOR vNormal = XMLoadFloat3(&_in.vtx.v3Normal);
		XMFLOAT4 v4Ambient = pLight->GetAmbientColor();


		if (type == LIGHT_TYPE::DIRECTIONAL)
		{
			pLight->ComputeDirectionalLight(v4Diffuse);

		}
		else if (type == LIGHT_TYPE::POINT)
		{
			pLight->ComputePointLight(v4Diffuse);
		}
		else if (type == LIGHT_TYPE::SPOT)
		{
			pLight->ComputeSpotlight(v4Diffuse);
		}
		else
		{
			pLight->ComputePhong(v4Diffuse);

		}

		v4Final = pLight->GetComputedColor();


		colFinal.rgbRed = min(BYTE(v4Final.x * 255.f), 255);
		colFinal.rgbGreen = min(BYTE(v4Final.y * 255.f), 255);
		colFinal.rgbBlue = min(BYTE(v4Final.z * 255.f), 255);
		colFinal.rgbReserved = 255;


	}

	return colFinal;
}
