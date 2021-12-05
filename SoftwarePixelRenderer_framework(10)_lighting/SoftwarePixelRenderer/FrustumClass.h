#pragma once

#include <DirectXMath.h>
#include "CFace.h"

using namespace DirectX;

class CFrustumClass
{
public:
	CFrustumClass();
	CFrustumClass(const CFrustumClass &rFrustum);
	~CFrustumClass();

	void ConstructFrustum(const float fScreenDepth, const XMMATRIX &ProjectionMatrix, const XMMATRIX &ViewMatrix);
	
	bool CheckPoint(const float x, const float y, const float z);
	bool CheckPoint(const XMVECTOR vPoint);
	bool CheckCube(const float xCenter, const float yCenter, const float zCenter, const float fRadius);
	bool CheckCube(const XMVECTOR vCube[8]);
	bool CheckSphere(const float xCenter, const float yCenter, const float zCenter, const float fRadius);
	bool CheckRectangle(const float xCenter, const float yCenter, const float zCenter, const float xSize, const float ySize, const float zSize);
	void FaceClipAgainstPlane(const CFace &face,list<CFace>& lstOut);

protected:
	VERTEX IntersectPlane(XMFLOAT4& v4Plane, VERTEX* vtxStart, VERTEX* vtxEnd);

private:
	XMFLOAT4 m_v4Planes[6];
};

