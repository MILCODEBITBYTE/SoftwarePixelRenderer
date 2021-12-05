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

void GetFaceValue(const wchar_t* szFaceInfo, const size_t count, vector<DWORD>& vecOut)
{
	vecOut.clear();

	wchar_t szValue[20] = { 0, };

	int iValueIndex = 0;
	for (size_t i = 0; i < count; ++i)
	{
		if (szFaceInfo[i] == L'/' || szFaceInfo[i] == L'\r' || szFaceInfo[i] == '\n' || szFaceInfo[i] == '\0')
		{
			szValue[iValueIndex] = L'\0';
			vecOut.push_back((DWORD)_wtol(szValue));

			iValueIndex = 0;
		}
		else
			szValue[iValueIndex++] = szFaceInfo[i];
	}
}