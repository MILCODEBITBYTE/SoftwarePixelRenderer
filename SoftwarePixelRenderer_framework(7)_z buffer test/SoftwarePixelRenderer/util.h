#pragma once

#include <DirectXMath.h>

using namespace DirectX;

inline XMFLOAT2 operator + (const XMFLOAT2& v1, const XMFLOAT2& v2) { return XMFLOAT2(v1.x + v2.x, v1.y + v2.y); }
inline XMFLOAT2 operator - (const XMFLOAT2& v1, const XMFLOAT2& v2) { return XMFLOAT2(v1.x - v2.x, v1.y - v2.y); }
inline XMFLOAT2 operator * (const XMFLOAT2& v1, const XMFLOAT2& v2) { return XMFLOAT2(v1.x * v2.x, v1.y * v2.y); }
inline XMFLOAT2 operator / (const XMFLOAT2& v1, const XMFLOAT2& v2) { return XMFLOAT2(v1.x / v2.x, v1.y / v2.y); }

inline XMFLOAT3 operator + (const XMFLOAT3& v1, const XMFLOAT3& v2) { return XMFLOAT3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
inline XMFLOAT3 operator - (const XMFLOAT3& v1, const XMFLOAT3& v2) { return XMFLOAT3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
inline XMFLOAT3 operator * (const XMFLOAT3& v1, const XMFLOAT3& v2) { return XMFLOAT3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z); }
inline XMFLOAT3 operator / (const XMFLOAT3& v1, const XMFLOAT3& v2) { return XMFLOAT3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z); }
inline XMFLOAT3 operator += (XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x += v2.x; v1.y += v2.y; v1.z += v2.z; return v1; }
inline XMFLOAT3 operator -= (XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x -= v2.x; v1.y -= v2.y; v1.z -= v2.z; return v1; }
inline XMFLOAT3 operator *= (XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x *= v2.x; v1.y *= v2.y; v1.z *= v2.z; return v1; }
inline XMFLOAT3 operator /= (XMFLOAT3& v1, const XMFLOAT3& v2) { v1.x /= v2.x; v1.y /= v2.y; v1.z /= v2.z; return v1; }
inline bool operator == (XMFLOAT3& v1, const XMFLOAT3& v2) { return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z; }


inline XMFLOAT4 operator + (const XMFLOAT4& v1, const float fScale) { return XMFLOAT4(v1.x + fScale, v1.y + fScale, v1.z + fScale, v1.w + fScale); }
inline XMFLOAT4 operator - (const XMFLOAT4& v1, const float fScale) { return XMFLOAT4(v1.x - fScale, v1.y - fScale, v1.z - fScale, v1.w - fScale); }
inline XMFLOAT4 operator * (const XMFLOAT4& v1, const float fScale) { return XMFLOAT4(v1.x * fScale, v1.y * fScale, v1.z * fScale, v1.w * fScale); }
inline XMFLOAT4 operator / (const XMFLOAT4& v1, const float fScale) { return XMFLOAT4(v1.x / fScale, v1.y / fScale, v1.z / fScale, v1.w / fScale); }

inline XMFLOAT4 operator + (const XMFLOAT4& v1, const XMFLOAT4& v2) { return XMFLOAT4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w); }
inline XMFLOAT4 operator - (const XMFLOAT4& v1, const XMFLOAT4& v2) { return XMFLOAT4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w); }
inline XMFLOAT4 operator * (const XMFLOAT4& v1, const XMFLOAT4& v2) { return XMFLOAT4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w); }
inline XMFLOAT4 operator / (const XMFLOAT4& v1, const XMFLOAT4& v2) { return XMFLOAT4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w); }
inline XMFLOAT4 operator += (XMFLOAT4& v1, const XMFLOAT4& v2) { v1.x += v2.x; v1.y += v2.y; v1.z += v2.z; v1.w += v2.w;  return v1; }
inline XMFLOAT4 operator -= (XMFLOAT4& v1, const XMFLOAT4& v2) { v1.x -= v2.x; v1.y -= v2.y; v1.z -= v2.z; v1.w -= v2.w; return v1; }
inline XMFLOAT4 operator *= (XMFLOAT4& v1, const XMFLOAT4& v2) { v1.x *= v2.x; v1.y *= v2.y; v1.z *= v2.z; v1.w *= v2.w; return v1; }
inline XMFLOAT4 operator /= (XMFLOAT4& v1, const XMFLOAT4& v2) { v1.x /= v2.x; v1.y /= v2.y; v1.z /= v2.z; v1.w /= v2.w; return v1; }
inline bool operator == (XMFLOAT4& v1, const XMFLOAT4& v2) { return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w; }

inline bool operator == (const D2D1_POINT_2F& pt1, const D2D1_POINT_2F& pt2) { return (pt1.x == pt2.x && pt1.y == pt2.y); }
inline D2D1_POINT_2F operator + (const D2D1_POINT_2F& pt1, const D2D1_POINT_2F& pt2) { return D2D1::Point2F(pt1.x + pt2.x, pt1.y + pt2.y); }
inline D2D1_POINT_2F operator - (const D2D1_POINT_2F& pt1, const D2D1_POINT_2F& pt2) { return D2D1::Point2F(pt1.x - pt2.x, pt1.y - pt2.y); }
inline D2D1_POINT_2F operator / (const D2D1_POINT_2F& pt1, const D2D1_POINT_2F& pt2) { return D2D1::Point2F(pt1.x / pt2.x, pt1.y / pt2.y); }
inline D2D1_POINT_2F operator * (const D2D1_POINT_2F& pt1, const D2D1_POINT_2F& pt2) { return D2D1::Point2F(pt1.x * pt2.x, pt1.y * pt2.y); }

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


typedef struct _tagEdge
{
	D2D1_POINT_2F ptStart;
	D2D1_POINT_2F ptEnd;

	bool operator == (const _tagEdge& edge)
	{
		if (ptStart == edge.ptStart && ptEnd == edge.ptEnd ||
			ptStart == edge.ptEnd && ptEnd == edge.ptStart)
			return true;

		return false;
	}
}EDGE;


extern void GetLineValue(const wchar_t* szLine, const size_t count, vector<wstring>& vecOut);


extern bool CheckTriangle(const XMFLOAT3* pTri, const XMFLOAT3 v3Pos, const XMFLOAT3 v3Dir);
