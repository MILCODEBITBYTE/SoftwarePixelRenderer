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

//void SortVertexInFace(CFace& in, VERTEX* pVertices[3], bool bInverse = false)
//{
//
//	VERTEX* pTemp;
//
//	for (int i = 0; i < 2; ++i)
//	{
//		for (int j = i + 1; j < 3; ++j)
//		{
//			if (bInverse)
//			{
//				if (pVertices[i]->uiIndex < pVertices[j]->uiIndex)
//				{
//					pTemp = pVertices[i];
//					pVertices[i] = pVertices[j];
//					pVertices[j] = pTemp;
//				}
//			}
//			else
//			{
//				if (pVertices[i]->uiIndex > pVertices[j]->uiIndex)
//				{
//					pTemp = pVertices[i];
//					pVertices[i] = pVertices[j];
//					pVertices[j] = pTemp;
//				}
//			}
//			
//		}
//	}
//
//	in.SetFace(*pVertices[0], *pVertices[1], *pVertices[2]);
//}


void SortVertexInFace(CFace& in, VERTEX* pVertices[3], const XMFLOAT3 v3FaceNormal)
{

	VERTEX* pTemp;

	for (int i = 0; i < 2; ++i)
	{
		for (int j = i + 1; j < 3; ++j)
		{
			if (pVertices[i]->v3Vtx.x > pVertices[j]->v3Vtx.x)
			{
				pTemp = pVertices[i];
				pVertices[i] = pVertices[j];
				pVertices[j] = pTemp;
			}
		}
	}

	XMVECTOR vBA, vBC, vDir, vNormal;

	XMFLOAT3 v3BA = pVertices[0]->v3Vtx - pVertices[1]->v3Vtx;
	XMFLOAT3 v3BC = pVertices[2]->v3Vtx - pVertices[1]->v3Vtx;

	vBA = XMVector3Normalize(XMLoadFloat3(&v3BA));
	vBC = XMVector3Normalize(XMLoadFloat3(&v3BC));

	vDir = XMVector3Cross(vBA, vBC);
	vNormal = XMLoadFloat3(&v3FaceNormal);

	float fTheta = XMVectorGetX(XMVector3Dot(vDir, vNormal));

	if (fTheta < 0)
		in.SetFace(*pVertices[0], *pVertices[2], *pVertices[1]);
	else
		in.SetFace(*pVertices[0], *pVertices[1], *pVertices[2]);

}

void CFrustumClass::FaceClipAgainstPlane(const CFace &face, list<CFace>& lstOut)
{
	int iInside = 0;;
	XMFLOAT4 v4PlaneDot;
	XMVECTOR vPlane;
	VERTEX* pVertices;
	XMFLOAT3 v3Normal;
	vector<pair<int, int>> vecOutside;
	list<CFace> lstClip;

	lstClip.push_back(face);

	size_t tNewFace = 1;
	size_t tAddFace = 0;
	list<CFace>::iterator iter;
	size_t tVtxCount = 0;

	int indicies[3];
	VERTEX* vtxSort[3];

	vector<VERTEX*> vecLink;

	for (int j = 0; j < 6; ++j)
	{
		vPlane = XMLoadFloat4(&m_v4Planes[j]);
		

		while (tNewFace > 0)
		{
			CFace check = lstClip.front();
			check.MakeNormal();
			check.GetNormal(&v3Normal);
			lstClip.pop_front();
			tNewFace--;

			pVertices = check.GetVertices();
			tVtxCount = check.GetVertexCount();

			//평면당 정점들 테스트
			vecOutside.clear();
			iInside = 0;
			for (size_t i = 0; i < tVtxCount; ++i)
			{
				XMStoreFloat4(&v4PlaneDot, XMPlaneDotCoord(vPlane, XMVectorSet(pVertices[i].v3Vtx.x, pVertices[i].v3Vtx.y, pVertices[i].v3Vtx.z, 1)));

				if (v4PlaneDot.x < FLT_EPSILON)
				{
					vecOutside.push_back(make_pair((int)i, j));
				}
				else
					iInside++;
			}

			size_t tOutsideCnt = vecOutside.size();
			
			//1개의 새로운 FACE 생성
			if (tOutsideCnt == 1 && iInside == 2)
			{
				CFace NewFace;
				VERTEX *pStart, *pEnd, IntersectA, IntersectB;

				indicies[0] = vecOutside[0].first;
				indicies[1] = (vecOutside[0].first + 1) % tVtxCount;
				indicies[2] = (vecOutside[0].first + 2) % tVtxCount;

				pStart = &pVertices[indicies[0]];
				pEnd = &pVertices[indicies[1]];

				IntersectA = IntersectPlane(m_v4Planes[vecOutside[0].second], pStart, pEnd);

				vtxSort[0] = &IntersectA;
				vtxSort[1] = pEnd;
				vtxSort[2] = &pVertices[indicies[2]];

				SortVertexInFace(NewFace, vtxSort, v3Normal);

				lstClip.push_back(NewFace);

				pEnd = &pVertices[indicies[2]];

				IntersectB = IntersectPlane(m_v4Planes[vecOutside[0].second], pStart, pEnd);
				
				IntersectB.uiIndex = abs(int(pEnd->uiIndex + IntersectA.uiIndex) - 3);

				vtxSort[0] = &IntersectA;
				vtxSort[1] = pEnd;
				vtxSort[2] = &IntersectB;

				SortVertexInFace(NewFace, vtxSort, v3Normal);

				lstClip.push_back(NewFace);


			}
			//2개의 새로운 FACE 생성
			else if (tOutsideCnt == 2 && iInside == 1)
			{
				CFace NewFace;
				VERTEX* pStart, * pEnd, IntersectA, IntersectB;

				indicies[0] = vecOutside[0].first;
				indicies[1] = vecOutside[1].first;
				indicies[2] = abs(vecOutside[0].first + vecOutside[1].first - 3);

				pStart = &pVertices[indicies[0]];
				pEnd = &pVertices[indicies[2]];

				IntersectA = IntersectPlane(m_v4Planes[vecOutside[0].second], pStart, pEnd);

				pStart = &pVertices[indicies[1]];
				IntersectB = IntersectPlane(m_v4Planes[vecOutside[1].second], pStart, pEnd);

				vtxSort[0] = &IntersectA;
				vtxSort[1] = &IntersectB;
				vtxSort[2] = pEnd;

				SortVertexInFace(NewFace, vtxSort, v3Normal);

				lstClip.push_back(NewFace);

			}
			else if (tOutsideCnt == 0 && iInside == 3)
			{
				lstClip.push_back(check);
			}

		}
		tNewFace = lstClip.size();
	}

	lstOut.insert(lstOut.end(), lstClip.begin(), lstClip.end());

}

VERTEX CFrustumClass::IntersectPlane(XMFLOAT4& v4Plane, VERTEX* vtxStart, VERTEX* vtxEnd)
{
	VERTEX vtxNew;
	XMVECTOR vPlane, vStart, vEnd;
	XMVECTOR vIntersect;
	vPlane = XMLoadFloat4(&v4Plane);
	vStart = XMLoadFloat3(&vtxStart->v3Vtx);
	vEnd = XMLoadFloat3(&vtxEnd->v3Vtx);

	float fA, fB, fLen, fRate;

	fA = XMVectorGetX(XMPlaneDotCoord(vPlane, vStart));
	fB = XMVectorGetX(XMPlaneDotCoord(vPlane, vEnd));

	fLen = abs(fB - fA);
	fRate = abs(fA) / fLen;

	//Vertex
	vStart = vStart * (1.f - fRate);
	vEnd = vEnd * fRate;

	vIntersect = XMVectorAdd(vStart, vEnd);

	XMStoreFloat3(&vtxNew.v3Vtx, vIntersect);

	//Normal
	vtxNew.v3Normal = vtxStart->v3Normal;

	//Color
	XMFLOAT4 v4ColStart, v4ColEnd;
	v4ColStart = vtxStart->v4Color * (1.f - fRate);
	v4ColEnd = vtxEnd->v4Color * fRate;

	vtxNew.v4Color = v4ColStart + v4ColEnd;

	vtxNew.uiIndex = vtxStart->uiIndex;

	return vtxNew;
}

