#include "pch.h"
#include "util.h"

XMVECTOR _stdcall FaceNormal(XMVECTOR& v1, XMVECTOR& v2, XMVECTOR& v3)
{
	XMVECTOR p1, p2;
	p1 = v1 - v2;
	p2 = v3 - v2;

	return XMVector3Cross(p1, p2);
}

XMFLOAT3 _stdcall FaceNormal(XMFLOAT3& v31, XMFLOAT3& v32, XMFLOAT3& v33)
{
	XMFLOAT3 v3Normal;
	XMFLOAT3 v3Dir1, v3Dir2;
	XMVECTOR p1, p2;

	v3Dir1 = v31 - v32;
	v3Dir2 = v33 - v32;
	p1 = XMLoadFloat3(&v3Dir1);
	p2 = XMLoadFloat3(&v3Dir2);

	XMStoreFloat3(&v3Normal, XMVector3Cross(p1, p2));
	return v3Normal;
}

float _stdcall DotToCosf(XMVECTOR& v1, XMVECTOR& v2)
{
	XMFLOAT3 v31, v32;

	XMStoreFloat3(&v31, v1);
	XMStoreFloat3(&v32, v2);

	//Dot(v1, v2) * cos(theta) = x1x2 + y1y2 + z1z2

	return  acosf(v31.x * v32.x + v31.y * v32.y + v31.z * v32.z);
}

float _stdcall DotToCosf(XMFLOAT3& v31, XMFLOAT3& v32)
{
	return  acosf(v31.x * v32.x + v31.y * v32.y + v31.z * v32.z);
}

void GetLineValue(const wchar_t* szLine, const size_t count, vector<wstring>& vecOut)
{
	vecOut.clear();

	wchar_t szValue[100] = { 0, };

	int iValueIndex = 0;
	for (size_t i = 0; i < count; ++i)
	{
		if (szLine[i] == L' ' || szLine[i] == L'\r' || szLine[i] == '\n' || szLine[i] == '\0')
		{
			szValue[iValueIndex] = L'\0';
			vecOut.push_back(szValue);

			iValueIndex = 0;
		}
		else
			szValue[iValueIndex++] = szLine[i];
	}

}

bool CheckTriangle(const XMFLOAT3* pTri, const XMFLOAT3 v3Pos, const XMFLOAT3 v3Dir)
{
    XMVECTOR vOrg, vDir;

    XMFLOAT3 e1, e2;
    e1 = pTri[1] - pTri[0];
    e2 = pTri[2] - pTri[0];

    XMVECTOR vE1, vE2;
    XMVECTOR pVec, tVec;

    vOrg = XMLoadFloat3(&v3Pos);
    vDir = XMLoadFloat3(&v3Dir);
    vE1 = XMLoadFloat3(&e1);
    vE2 = XMLoadFloat3(&e2);

    pVec = XMVector3Cross(vDir, vE2);
    float fDet = XMVectorGetX(XMVector3Dot(vE1, pVec));

    if (fDet > FLT_EPSILON)
    {
        tVec = vOrg - XMLoadFloat3(&pTri[0]);
    }
    else
    {
        tVec = XMLoadFloat3(&pTri[0]) - vOrg;
        fDet = -fDet;
    }

    if (fDet < FLT_EPSILON) return false;


    float u, v;
    u = XMVectorGetX(XMVector3Dot(tVec, pVec));

    if (u < FLT_EPSILON || u > fDet)
        return false;

    XMVECTOR qVec;
    qVec = XMVector3Cross(tVec, vE1);

    v = XMVectorGetX(XMVector3Dot(vDir, qVec));

    float fAdd = u + v;

    if (v < FLT_EPSILON || fAdd > fDet)
        return false;

    /*
    float t = XMVectorGetX(XMVector3Dot(vE2, qVec));
    float fInvDet = 1.0f / fDet;
    t *= fInvDet;
    u *= fInvDet;
    v *= fInvDet;
    */

    return true;
}
