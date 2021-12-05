#pragma once
#include "CShader.h"
class CStencilShader : public CShader
{
public:

	void Render(CFace* pFace, CMaterial* pMateril, CLight* pLight, const RENDER_STATE state = RENDER_STATE::FLAT, const bool bLight = true, const bool bZTest = true, const bool bStencil = true) override;
protected:
	void DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, CLight* pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil) override;
	void DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, CLight* pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil) override;

private:
	void FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil) override;
	void FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil) override;
};

