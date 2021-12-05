#pragma once

#include "CMesh.h"

typedef struct _tagBoundMinMax
{
	XMFLOAT3 min;
	XMFLOAT3 max;
}BOUND_MIN_MAX;

class CBoundbox : public CMesh
{
public:
	CBoundbox();
	CBoundbox(const CBoundbox& box);
	~CBoundbox();

	CBoundbox* Clone() override;

	void CalcBound(VERTEX* pVertices, const size_t tVtxCount);
	void CalcBound(VERTEX** ppVertices, const size_t tVtxCount);
	void MakeVolume();

	void Reset();
	void GetBoundMinMax(BOUND_MIN_MAX *pBox);

private:
	

private:
	BOUND_MIN_MAX m_Box;
};

