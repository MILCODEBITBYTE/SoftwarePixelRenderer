#pragma once

#include "CFace.h"
#include "CMaterial.h"
#include "CBresenhamLine.h"
#include "CLight.h"

const enum class RENDER_STATE { BOUNDBOX, TEST, FLAT, TEXTURE, GOURAUD, PHONG, ANISOTROPIC, BLINN, METAL, SKIN };

class CShader
{
public:
	CShader();
	virtual ~CShader();

	void Initialize(IWICBitmap* pScreenBuffer, float* pZbuffer = nullptr, float* pStencilBuffer = nullptr);
	void SetWireframe(const bool bShow);
	bool IsWireframe() const;
	virtual void Render(CFace* pFace, CMaterial* pMateril, CLight *pLight, const RENDER_STATE state = RENDER_STATE::FLAT, const bool bLight = true, const bool bZTest=true, const bool bStencil=true);

	void SetModelMatrix(const XMFLOAT4X4& matModel);
	void SetModelMatrix(const XMMATRIX& matModel);
	void SetWorldMatrix(const XMFLOAT4X4& matWorld);
	void SetWorldMatrix(const XMMATRIX& matWorld);
	void SetViewMatrix(const XMFLOAT4X4& matView);
	void SetViewMatrix(const XMMATRIX& matView);
	void SetProjectionMatrix(const XMFLOAT4X4& matProjection);
	void SetProjectionMatrix(const XMMATRIX& matProjection);
	void SetViewportMatrix(const XMFLOAT4X4& matViewport);
	void SetViewportMatrix(const XMMATRIX& matViewport);

	bool ZTest(float* pBuffer, const UINT uiOffset, const float fZ, const bool bWrite = true);
protected:
	virtual void DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil);
	virtual void DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, CLight* pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil);

	virtual VERTEX VertexShading(const VERTEX& _in, CLight* pLight, const bool bLight);
	virtual RGBQUAD PixelShading(BYTE* pSrc, const UINT uiSrcStride, const UINT uiSrcWidth, const UINT uiSrcHeight, const int iChannel, const VERTEX& _in, CLight* pLight, const bool bLight);

	virtual void FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CWicBitmap* pTex, CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil);
	virtual void FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CWicBitmap* pTex, CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil);

	virtual void FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil);
	virtual void FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil);

	virtual void DrawTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil);
	virtual void DrawLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd,  CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil);

protected:

	IWICBitmap* m_pScreenBuffer;
	float*		m_pZBuffer;
	float*		m_pStencilBuffer;

	XMFLOAT4X4 m_matModel;
	XMFLOAT4X4 m_matWorld;
	XMFLOAT4X4 m_matView;
	XMFLOAT4X4 m_matProj;
	XMFLOAT4X4 m_matViewport;

	bool m_bWireframe;
};

