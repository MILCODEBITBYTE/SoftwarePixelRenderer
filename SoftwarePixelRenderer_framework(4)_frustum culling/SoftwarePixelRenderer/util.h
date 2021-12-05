#pragma once

#include <DirectXMath.h>

using namespace DirectX;

inline XMFLOAT3 operator + (const XMFLOAT3& v1, const XMFLOAT3& v2) { return XMFLOAT3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
inline XMFLOAT3 operator - (const XMFLOAT3& v1, const XMFLOAT3& v2) { return XMFLOAT3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
inline XMFLOAT3 operator * (const XMFLOAT3& v1, const XMFLOAT3& v2) { return XMFLOAT3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z); }
inline XMFLOAT3 operator / (const XMFLOAT3& v1, const XMFLOAT3& v2) { return XMFLOAT3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z); }
inline XMFLOAT3 operator += (XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x += v2.x; v1.y += v2.y; v1.z += v2.z; return v1; }
inline XMFLOAT3 operator -= (XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x -= v2.x; v1.y -= v2.y; v1.z -= v2.z; return v1; }
inline XMFLOAT3 operator *= (XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x *= v2.x; v1.y *= v2.y; v1.z *= v2.z; return v1; }
inline XMFLOAT3 operator /= (XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x /= v2.x; v1.y /= v2.y; v1.z /= v2.z; return v1; }
inline bool operator == (const XMFLOAT3& v1, const XMFLOAT3& v2) { return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z; }

inline bool operator == (const D2D1_POINT_2F& pt1, const D2D1_POINT_2F& pt2) { return (pt1.x == pt2.x && pt1.y == pt2.y); }

inline XMMATRIX XMMatrixViewport(int iScreenWidth, int iScreenHeight, int iMoveX = 0, int iMoveY = 0)
{
	/*
	XMFLOAT4X4 matViewport;
	matViewport._11 = iScreenWidth / 2.f;
	matViewport._22 = iScreenWidth / 2.f;
	matViewport._14 = iMoveX + iScreenWidth / 2.f;
	matViewport._24 = iMoveY + iScreenHeight / 2.f;
	matViewport._33 = 1;
	matViewport._44 = 1;
	*/

	return XMMatrixSet(	iScreenWidth / 2.f, 0, 0, 0, 
						0, -iScreenHeight / 2.f, 0, 0, 
						0, 0, 1, 0, 
						iMoveX + iScreenWidth / 2.f, iMoveY + iScreenHeight / 2.f, 0, 1);

}

extern XMVECTOR _stdcall FaceNormal(XMVECTOR& v1, XMVECTOR& v2, XMVECTOR& v3);
extern XMFLOAT3 _stdcall FaceNormal(XMFLOAT3& v31, XMFLOAT3& v32, XMFLOAT3& v33);
extern float _stdcall DotToCosf(XMVECTOR& v1, XMVECTOR& v2);
extern float _stdcall DotToCosf(XMFLOAT3& v31, XMFLOAT3& v32);

extern void GetLineValue(const wchar_t* szLine, const size_t count, vector<wstring>& vecOut);
extern void GetFaceValue(const wchar_t* szFaceInfo, const size_t count, vector<DWORD>& vecOut);