#pragma once

#include "CShader.h"

class CTextureShader : public CShader
{
public:
	CTextureShader();
	~CTextureShader();

	void Render(CFace* pFace, CMaterial* pMaterial,const RENDER_STATE state = RENDER_STATE::FLAT,  const bool bZTest = true) override;

	void DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial,  const wstring strMaterialName,  const RENDER_STATE state,  const bool bZTest) override;
	void DrawQuad(VERTEX* pVertices, CMaterial* pMaterial,  const wstring strMaterialName, const RENDER_STATE state, const bool bZTest) override;

private:

	void FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CWicBitmap* pTex,  const bool bZTest) override;
	void FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CWicBitmap* pTex, const bool bZTest) override;

};
