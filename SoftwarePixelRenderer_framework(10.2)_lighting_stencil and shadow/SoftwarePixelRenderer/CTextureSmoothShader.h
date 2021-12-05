#pragma once

#include "CShader.h"

typedef struct _tagVertexInputType
{
	VERTEX vtx;
	XMMATRIX matLightView;
	XMFLOAT3 v3LightPos;
}VERTEX_INPUT_TYPE;

typedef struct _tagPixelInputType
{
	VERTEX vtx;
	float fStencilZ;
	XMFLOAT3 v3LightViewPos;
}PIXEL_INPUT_TYPE;

class CTextureSmoothShader : public CShader
{
public:
	CTextureSmoothShader();
	~CTextureSmoothShader();

	void Render(CFace* pFace, CMaterial* pMaterial, CLight *pLight, const RENDER_STATE state = RENDER_STATE::FLAT, const bool bLight=true, const bool bZTest = true, const bool bStencil = true) override;

	void DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName,const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil) override;
	void DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, CLight *pLight, const wstring strMaterialName, const RENDER_STATE state, const bool bLight, const bool bZTest, const bool bStencil) override;


private:

	PIXEL_INPUT_TYPE VertexShading(const VERTEX_INPUT_TYPE& _in, CLight* pLight, const bool bLight);
	RGBQUAD PixelShading(BYTE* pSrc, const UINT uiSrcStride, const UINT uiSrcWidth, const UINT uiSrcHeight, const int iChannel, const PIXEL_INPUT_TYPE& _in, CLight* pLight, const bool bLight);

	void FillTrilinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pVertices[3], CWicBitmap* pTex, CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil) override;
	void FillLinearInterpolation(BYTE* pBack, const UINT uiStride, const VERTEX* pStart, const VERTEX* pEnd, CWicBitmap* pTex, CLight* pLight, const bool bLight, const bool bZTest, const bool bStencil) override;



private:
	XMMATRIX m_matInvToModel;
	XMMATRIX m_matToProj;
	XMMATRIX m_matToViewport;
};
