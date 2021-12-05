#include "pch.h"
#include "CShader.h"
#include <functional>
#include <algorithm>

CShader::CShader()
{
	m_pScreenBuffer = nullptr;
	m_pZBuffer = nullptr;
	m_bWireframe = false;
}

CShader::~CShader()
{
}

void CShader::Initialize(IWICBitmap* pScreenBuffer, float* pZBuffer)
{
	m_pScreenBuffer = pScreenBuffer;
	m_pZBuffer = pZBuffer;
}

void CShader::SetWireframe(const bool bShow)
{
	m_bWireframe = bShow;
}

bool CShader::IsWireframe() const
{
	return m_bWireframe;
}

void CShader::Render(CFace* pFace, CMaterial* pMateril,  const RENDER_STATE state, const bool bZTest)
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

bool CShader::ZTest(float* pBuffer, const UINT uiOffset, const float fZ)
{

	if (fZ >= FLT_EPSILON && pBuffer[uiOffset] > fZ)
	//if (pBuffer[uiOffset] > fZ)
	{
		pBuffer[uiOffset] = fZ;
		return true;
	}

	return false;

}

void CShader::DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName,const RENDER_STATE state,  const bool bZTest)
{
}

void CShader::DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const RENDER_STATE state, const bool bZTest)
{
}


void CShader::FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CWicBitmap* pTex,  const bool bZTest)
{
}

void CShader::FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CWicBitmap* pTex, const bool bZTest)
{
}

void CShader::FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3],  const bool bZTest)
{
}

void CShader::FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, const bool bZTest)
{
}

void CShader::DrawTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3],  const bool bZTest)
{
}

void CShader::DrawLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, const bool bZTest)
{
}

