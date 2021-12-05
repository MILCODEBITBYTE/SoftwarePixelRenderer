#include "pch.h"
#include "CShader.h"
#include <functional>
#include <algorithm>

CShader::CShader()
{
	m_pScreenBuffer = nullptr;
	m_bWireframe = false;
}

CShader::~CShader()
{
}

void CShader::Initialize(IWICBitmap* pScreenBuffer)
{
	m_pScreenBuffer = pScreenBuffer;
}

void CShader::SetWireframe(const bool bShow)
{
	m_bWireframe = bShow;
}

bool CShader::IsWireframe() const
{
	return m_bWireframe;
}

void CShader::Render(CFace* pFace, CMaterial* pMateril, const RENDER_STATE state)
{
}

void CShader::SetModelMatrix(const XMFLOAT4X4& matModel)
{
	m_matModel = matModel;
}

void CShader::SetModelMatrix(const XMMATRIX& matModel)
{
	XMStoreFloat4x4(&m_matModel, matModel);
}

void CShader::SetWorldMatrix(const XMFLOAT4X4& matWorld)
{
	m_matWorld = matWorld;
}

void CShader::SetWorldMatrix(const XMMATRIX& matWorld)
{
	XMStoreFloat4x4(&m_matWorld, matWorld);
}

void CShader::SetViewMatrix(const XMFLOAT4X4& matView)
{
	m_matView = matView;
}

void CShader::SetViewMatrix(const XMMATRIX& matView)
{
	XMStoreFloat4x4(&m_matView, matView);
}

void CShader::SetProjectionMatrix(const XMFLOAT4X4& matProjection)
{
	m_matProj = matProjection;
}

void CShader::SetProjectionMatrix(const XMMATRIX& matProjection)
{
	XMStoreFloat4x4(&m_matProj, matProjection);
}

void CShader::SetViewportMatrix(const XMFLOAT4X4& matViewport)
{
	m_matViewport = matViewport;
}

void CShader::SetViewportMatrix(const XMMATRIX& matViewport)
{
	XMStoreFloat4x4(&m_matViewport, matViewport);
}

void CShader::DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const int iScreenWidth, const int iScreenHeight, const RENDER_STATE state)
{
}

void CShader::DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const int iScreenWidth, const int iScreenHeight, const RENDER_STATE state)
{
}



void CShader::DrawFlatLine(BYTE* pBuffer, const UINT uiStride, const XMFLOAT4 v4Col, const VERTEX* pStart, const VERTEX* pEnd, const int iScreenWidth, const int iScreenHeight)
{
	CPoint ptStart, ptEnd;

	ptStart = CPoint((int)roundf(pStart->v3Vtx.x), (int)roundf(pStart->v3Vtx.y));
	ptEnd = CPoint((int)roundf(pEnd->v3Vtx.x), (int)roundf(pEnd->v3Vtx.y));

	if (ptStart.x >= iScreenWidth)
		ptStart.x = iScreenWidth - 1;
	else if (ptStart.x < 0)
		ptStart.x = 0;

	if (ptStart.y >= iScreenHeight)
		ptStart.y = iScreenHeight - 1;
	else if (ptStart.y < 0)
		ptStart.y = 0;

	if (ptEnd.x < 0)
		ptEnd.x = 0;
	else if (ptEnd.x >= iScreenWidth)
		ptEnd.x = iScreenWidth - 1;

	if (ptEnd.y < 0)
		ptEnd.y = 0;
	else if (ptEnd.y >= iScreenHeight)
		ptEnd.y = iScreenHeight - 1;


	BYTE R, G, B;
	R = BYTE(255 * v4Col.x);
	G = BYTE(255 * v4Col.y);
	B = BYTE(255 * v4Col.z);

	CBresenhamLine line;

	line.Initialize(ptStart, ptEnd);

	long lBackIndex;
	long x, y;

	do
	{
		x = line.GetCurrentX();
		y = line.GetCurrentY();

		lBackIndex = y * uiStride + x * 4;

		pBuffer[lBackIndex + 0] = B;
		pBuffer[lBackIndex + 1] = G;
		pBuffer[lBackIndex + 2] = R;
		pBuffer[lBackIndex + 3] = 255;


	} while (line.MoveNext());

}

void CShader::DrawSmoothLine(BYTE* pBuffer, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, const int iScreenWidth, const int iScreenHeight)
{
	CPoint ptStart, ptEnd, ptDir;

	ptStart = CPoint((int)roundf(pStart->v3Vtx.x), (int)roundf(pStart->v3Vtx.y));
	ptEnd = CPoint((int)roundf(pEnd->v3Vtx.x), (int)roundf(pEnd->v3Vtx.y));

	if (ptStart.x >= iScreenWidth)
		ptStart.x = iScreenWidth - 1;
	else if (ptStart.x < 0)
		ptStart.x = 0;

	if (ptStart.y >= iScreenHeight)
		ptStart.y = iScreenHeight - 1;
	else if (ptStart.y < 0)
		ptStart.y = 0;

	if (ptEnd.x < 0)
		ptEnd.x = 0;
	else if (ptEnd.x >= iScreenWidth)
		ptEnd.x = iScreenWidth - 1;

	if (ptEnd.y < 0)
		ptEnd.y = 0;
	else if (ptEnd.y >= iScreenHeight)
		ptEnd.y = iScreenHeight - 1;

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

	long lBackIndex;

	do
	{
		x = line.GetCurrentX();
		y = line.GetCurrentY();

		fRate = line.GetCurrentRate();
		if (line.isLeft())
			fRate = 1.f - fRate;


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
