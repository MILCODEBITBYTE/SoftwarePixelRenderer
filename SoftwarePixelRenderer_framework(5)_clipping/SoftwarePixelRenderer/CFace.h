#pragma once
class CFace
{
public:
	CFace();
	CFace(const VERTEX& vtx1, const VERTEX& vtx2, const VERTEX& vtx3);
	CFace(const VERTEX& vtx1, const VERTEX& vtx2, const VERTEX& vtx3, const VERTEX& vtx4);
	~CFace();

	void SetFace(const VERTEX& vtx1, const VERTEX& vtx2, const VERTEX& vtx3);
	void SetFace(const VERTEX& vtx1, const VERTEX& vtx2, const VERTEX& vtx3, const VERTEX& vtx4);
	void SetFace(const VERTEX* pVertices, const int iVtxCount);
	size_t GetVertexCount() const;
	VERTEX* GetVertices();

	void MakeNormal();
	void GetNormal(XMVECTOR *pNormal);
	void GetNormal(XMFLOAT3 *pNormal);
	float CalcVectorDotCosf(const XMVECTOR& v);
	float CalcVectorDotCosf(const XMVECTOR& vNormal, const XMVECTOR& vLook);
	float CalcVectorDotCosf(const XMFLOAT3& v3);
	float CalcDotNormal(const XMFLOAT3& v3);
	float CalcDotNormal(const XMVECTOR& v);

private:
	vector<VERTEX> m_vecVertices;
	int m_iFaceInVertexCount;
};

