#include "pch.h"
#include "FrustumClass.h"



CFrustumClass::CFrustumClass()
{
}

CFrustumClass::CFrustumClass(const CFrustumClass & rFrustum)
{
}


CFrustumClass::~CFrustumClass()
{
}



void CFrustumClass::ConstructFrustum(const float screenDepth, const XMMATRIX &projectionMatrix, const XMMATRIX &viewMatrix)
{
	XMVECTOR vPlane;

	float zMinimum, r;
	XMMATRIX matrix;
	XMMATRIX matProj;
	XMFLOAT4X4 mat4Proj;
	XMFLOAT4X4 mat4Plane;
	XMStoreFloat4x4(&mat4Proj, projectionMatrix);

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -mat4Proj._43 / mat4Proj._33;
	r = screenDepth / (screenDepth - zMinimum);
	mat4Proj._33 = r;
	mat4Proj._43 = -r * zMinimum;

	matProj = XMLoadFloat4x4(&mat4Proj);

	// Create the frustum matrix from the view matrix and updated projection matrix.
	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
	XMStoreFloat4x4(&mat4Plane, matrix);

	// Calculate near plane of frustum.
	m_v4Planes[0].x = mat4Plane._14 + mat4Plane._13;
	m_v4Planes[0].y = mat4Plane._24 + mat4Plane._23;
	m_v4Planes[0].z = mat4Plane._34 + mat4Plane._33;
	m_v4Planes[0].w = mat4Plane._44 + mat4Plane._43;
	vPlane = XMLoadFloat4(&m_v4Planes[0]);
	XMStoreFloat4(&m_v4Planes[0], XMPlaneNormalize(vPlane));

	// Calculate far plane of frustum.
	m_v4Planes[1].x = mat4Plane._14 - mat4Plane._13;
	m_v4Planes[1].y = mat4Plane._24 - mat4Plane._23;
	m_v4Planes[1].z = mat4Plane._34 - mat4Plane._33;
	m_v4Planes[1].w = mat4Plane._44 - mat4Plane._43;
	vPlane = XMLoadFloat4(&m_v4Planes[1]);
	XMStoreFloat4(&m_v4Planes[1], XMPlaneNormalize(vPlane));

	// Calculate left plane of frustum.
	m_v4Planes[2].x = mat4Plane._14 + mat4Plane._11;
	m_v4Planes[2].y = mat4Plane._24 + mat4Plane._21;
	m_v4Planes[2].z = mat4Plane._34 + mat4Plane._31;
	m_v4Planes[2].w = mat4Plane._44 + mat4Plane._41;
	vPlane = XMLoadFloat4(&m_v4Planes[2]);
	XMStoreFloat4(&m_v4Planes[2], XMPlaneNormalize(vPlane));

	// Calculate right plane of frustum.
	m_v4Planes[3].x = mat4Plane._14 - mat4Plane._11;
	m_v4Planes[3].y = mat4Plane._24 - mat4Plane._21;
	m_v4Planes[3].z = mat4Plane._34 - mat4Plane._31;
	m_v4Planes[3].w = mat4Plane._44 - mat4Plane._41;
	vPlane = XMLoadFloat4(&m_v4Planes[3]);
	XMStoreFloat4(&m_v4Planes[3], XMPlaneNormalize(vPlane));

	// Calculate top plane of frustum.
	m_v4Planes[4].x = mat4Plane._14 - mat4Plane._12;
	m_v4Planes[4].y = mat4Plane._24 - mat4Plane._22;
	m_v4Planes[4].z = mat4Plane._34 - mat4Plane._32;
	m_v4Planes[4].w = mat4Plane._44 - mat4Plane._42;
	vPlane = XMLoadFloat4(&m_v4Planes[4]);
	XMStoreFloat4(&m_v4Planes[4], XMPlaneNormalize(vPlane));

	// Calculate bottom plane of frustum.
	m_v4Planes[5].x = mat4Plane._14 + mat4Plane._12;
	m_v4Planes[5].y = mat4Plane._24 + mat4Plane._22;
	m_v4Planes[5].z = mat4Plane._34 + mat4Plane._32;
	m_v4Planes[5].w = mat4Plane._44 + mat4Plane._42;
	vPlane = XMLoadFloat4(&m_v4Planes[5]);
	XMStoreFloat4(&m_v4Planes[5], XMPlaneNormalize(vPlane));


}



bool CFrustumClass::CheckPoint(const float x, const float y, const float z)
{
	int i;
	XMFLOAT4 v4PlaneDot;
	XMVECTOR vPlane;

	for (i = 0; i < 6; ++i)
	{
		vPlane = XMLoadFloat4(&m_v4Planes[i]);

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(x, y, z, 1)));
		if (v4PlaneDot.x < FLT_EPSILON)
			return false;
	}
	return true;
}

bool CFrustumClass::CheckPoint(const XMVECTOR vPos)
{
	int i;
	XMFLOAT4 v4PlaneDot;
	XMVECTOR vPlane;

	for (i = 0; i < 6; ++i)
	{
		vPlane = XMLoadFloat4(&m_v4Planes[i]);

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, vPos));
		if (v4PlaneDot.x < FLT_EPSILON)
			return false;
	}
	return true;
}

bool CFrustumClass::CheckCube(const XMVECTOR vCube[8])
{
	int i;
	XMFLOAT4 v4PlaneDot;
	XMVECTOR vPlane;
	for (i = 0; i < 6; ++i)
	{
		vPlane = XMLoadFloat4(&m_v4Planes[i]);

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, vCube[0]));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, vCube[1]));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, vCube[2]));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, vCube[3]));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, vCube[4]));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, vCube[5]));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, vCube[6]));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, vCube[7]));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		return false;

	}
	return true;
}

bool CFrustumClass::CheckCube(const float xCenter, const float yCenter, const float zCenter, const float fRadius)
{
	int i;
	XMFLOAT4 v4PlaneDot;
	XMVECTOR vPlane;
	for (i = 0; i < 6; ++i)
	{
		vPlane = XMLoadFloat4(&m_v4Planes[i]);

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter - fRadius, yCenter - fRadius, zCenter - fRadius, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter + fRadius, yCenter - fRadius, zCenter - fRadius, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter - fRadius, yCenter + fRadius, zCenter - fRadius, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter + fRadius, yCenter + fRadius, zCenter - fRadius, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter - fRadius, yCenter - fRadius, zCenter + fRadius, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter + fRadius, yCenter - fRadius, zCenter + fRadius, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter - fRadius, yCenter + fRadius, zCenter + fRadius, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter + fRadius, yCenter + fRadius, zCenter + fRadius, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		return false;

	}
	return true;
}

bool CFrustumClass::CheckSphere(const float xCenter, const float yCenter, const float zCenter, const float fRadius)
{
	int i = 0;
	XMFLOAT4 v4PlaneDot;
	XMVECTOR vPlane;

	for (i = 0; i < 6; ++i)
	{
		vPlane = XMLoadFloat4(&m_v4Planes[i]);
		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter, yCenter, zCenter, 1)));
		if (v4PlaneDot.x < -fRadius) return false;
	}
	

	return true;
}

bool CFrustumClass::CheckRectangle(const float xCenter, const float yCenter, const float zCenter, const float xSize, const float ySize, const float zSize)
{
	int i;
	XMFLOAT4 v4PlaneDot;
	XMVECTOR vPlane;

	for (i = 0; i < 6; ++i)
	{
		vPlane = XMLoadFloat4(&m_v4Planes[i]);

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter - xSize, yCenter - ySize, zCenter - zSize, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter + xSize, yCenter - ySize, zCenter - zSize, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter - xSize, yCenter + ySize, zCenter - zSize, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter - xSize, yCenter - ySize, zCenter + zSize, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter + xSize, yCenter + ySize, zCenter - zSize, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter + xSize, yCenter - ySize, zCenter + zSize, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter - xSize, yCenter + ySize, zCenter + zSize, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(xCenter + xSize, yCenter + ySize, zCenter + zSize, 1)));
		if (v4PlaneDot.x >= FLT_EPSILON) continue;

		return false;
	}
	return true;

}
