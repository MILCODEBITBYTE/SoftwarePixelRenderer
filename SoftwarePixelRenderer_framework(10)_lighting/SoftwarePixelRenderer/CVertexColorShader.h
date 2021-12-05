#pragma once

#include "CShader.h"



class CVertexColorShader : public CShader
{
public:
	CVertexColorShader();
	~CVertexColorShader();


	void Render(CFace* pFace, CMaterial* pMaterial, CLight *pLight, const RENDER_STATE state=RENDER_STATE::FLAT, const bool bLight = true, const bool bZTest=true) override;
protected:
	void DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest) override;
	void DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName,  const RENDER_STATE state, const bool bLight, const bool bZTest) override;
private:

	void FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CLight* pLight, const bool bLight, const bool bZTest) override;
	void FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CLight* pLight, const bool bLight, const bool bZTest) override;

	void DrawTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CLight* pLight, const bool bLight, const bool bZTest) override;
	void DrawLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CLight* pLight, const bool bLight, const bool bZTest) override;



private:
};

