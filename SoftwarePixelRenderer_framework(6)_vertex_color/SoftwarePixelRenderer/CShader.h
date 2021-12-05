#pragma once

#include "CFace.h"
#include "CMaterial.h"
#include "CBresenhamLine.h"

const enum class RENDER_STATE { BOUNDBOX, TEST, FLAT, TEXTURE, GOURAUD, PHONG, ANISOTROPIC, BLINN, METAL, SKIN };

class CShader
{
public:
	CShader();
	virtual ~CShader();

	void Initialize(IWICBitmap* pScreenBuffer);
	void SetWireframe(const bool bShow);
	bool IsWireframe() const;
	virtual void Render(CFace* pFace, CMaterial* pMateril, const RENDER_STATE state = RENDER_STATE::FLAT);

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

protected:
	virtual void DrawTriangle(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const int iScreenWidth, const int iScreenHeight, const RENDER_STATE state);
	virtual void DrawQuad(VERTEX* pVertices, CMaterial* pMaterial, const wstring strMaterialName, const int iScreenWidth, const int iScreenHeight, const RENDER_STATE state);

	void DrawFlatLine(BYTE* pBuffer, const UINT uiStride, const XMFLOAT4 v4Col, const VERTEX* pStart, const VERTEX* pEnd, const int iScreenWidth, const int iScreenHeight);
	void DrawSmoothLine(BYTE* pBuffer, const UINT uiStride,  const VERTEX* pStart, const VERTEX* pEnd, const int iScreenWidth, const int iScreenHeight);

protected:

	IWICBitmap* m_pScreenBuffer;

	XMFLOAT4X4 m_matModel;
	XMFLOAT4X4 m_matWorld;
	XMFLOAT4X4 m_matView;
	XMFLOAT4X4 m_matProj;
	XMFLOAT4X4 m_matViewport;

	bool m_bWireframe;
};

