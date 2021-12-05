#pragma once

#include "CShader.h"



class CVertexColorShader : public CShader
{
public:
	CVertexColorShader();
	~CVertexColorShader();


	void Render(CFace* pFace, CMaterial* pMaterial, const RENDER_STATE state=RENDER_STATE::FLAT) override;

protected:
	void DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const int iScreenWidth, const int iScreenHeight, const RENDER_STATE state) override;
	void DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const int iScreenWidth, const int iScreenHeight, const RENDER_STATE state) override;
private:
	void DrawTriTest(BYTE* pBack, const UINT stride, const VERTEX* pVertices, const int iScreenWidth, const int iScreenHeight);
	void FillTriTest(BYTE* pBack, const UINT stride, const VERTEX* pVertices, const int iScreenWidth, const int iScreenHeight);
	void DrawTriSmooth(BYTE* pBack, const UINT stride, const VERTEX* pVertices, const int iScreenWidth, const int iScreenHeight);
	void FillTriSmooth(BYTE* pBack, const UINT stride, const VERTEX* pVertices, const int iScreenWidth, const int iScreenHeight);
};

