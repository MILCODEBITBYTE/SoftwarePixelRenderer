#pragma once

#include "CShader.h"



class CVertexColorShader : public CShader
{
public:
	CVertexColorShader();
	~CVertexColorShader();


	void Render(CFace* pFace, CMaterial* pMaterial,const RENDER_STATE state=RENDER_STATE::FLAT) override;
protected:
	void DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const RENDER_STATE state) override;
	void DrawQuad(VERTEX* pVertices, CMaterial* pMaterial,  const wstring strMaterialName,  const RENDER_STATE state) override;
private:

	void FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3]) override;
	void FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd) override;

	void DrawTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3]) override;
	void DrawLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd) override;



private:
};

