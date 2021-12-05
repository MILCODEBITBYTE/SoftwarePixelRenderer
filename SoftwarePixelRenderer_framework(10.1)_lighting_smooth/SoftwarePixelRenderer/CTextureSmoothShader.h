#pragma once

#include "CShader.h"

class CTextureSmoothShader : public CShader
{
public:
	CTextureSmoothShader();
	~CTextureSmoothShader();

	void Render(CFace* pFace, CMaterial* pMaterial, CLight *pLight, const RENDER_STATE state = RENDER_STATE::FLAT, const bool bLight=true, const bool bZTest = true) override;

	void DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName,const RENDER_STATE state, const bool bLight, const bool bZTest) override;
	void DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest) override;


private:

	VERTEX VertexShading(const VERTEX& _in, CLight* pLight, const bool bLight) override;
	RGBQUAD PixelShading(BYTE* pSrc, const UINT uiSrcStride, const UINT uiSrcWidth, const UINT uiSrcHeight, const int iChannel, const VERTEX& _in, CLight* pLight, const bool bLight) override;

	void FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CWicBitmap* pTex, CLight* pLight, const bool bLight, const bool bZTest) override;
	void FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CWicBitmap* pTex, CLight* pLight, const bool bLight, const bool bZTest) override;



private:
	XMMATRIX m_matInvToModel;
	XMMATRIX m_matToProj;
	XMMATRIX m_matToViewport;
};
