#pragma once
class CFace
{
public:
	CFace();
	CFace(const XMFLOAT3 &v31, const XMFLOAT3& v32, const XMFLOAT3& v33);
	~CFace();

	void SetFace(const XMFLOAT3& v31, const XMFLOAT3& v32, const XMFLOAT3& v33);

	void MakeNormal();
	void GetNormal(XMVECTOR *pNormal);
	void GetNormal(XMFLOAT3 *pNormal);
	float CalcVectorDotCosf(const XMVECTOR& v);
	float CalcVectorDotCosf(const XMFLOAT3& v3);

private:
	XMFLOAT3	m_v31, m_v32, m_v33;
	XMFLOAT3	m_v3Normal;
};

