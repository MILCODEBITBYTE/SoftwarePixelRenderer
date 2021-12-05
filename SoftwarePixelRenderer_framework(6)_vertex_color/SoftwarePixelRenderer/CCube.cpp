#include "pch.h"
#include "CCube.h"

CCube::CCube()
{
}

CCube::CCube(const CCube& mesh)
	:CMesh(mesh)
{
}

CCube::~CCube()
{
}

CCube* CCube::Clone()
{
	return new CCube(*this);
}

void CCube::Initialize()
{
	m_vecModels.clear();
	m_vecModels.push_back(MESH_MODEL());

	MESH_MODEL* pModel = &m_vecModels.back();
	pModel->strName = L"Cube";

	vector<VERTEX>& vecVertices = pModel->vecVertices;
	vecVertices.resize(8);
	vector<CFace>& vecFacies = pModel->vecFacies;
	vector<DWORD> vecIndices;

	/*    
	*       0                     1
	*          式式式式式式式式式
	*        ㄞ弛               ㄞ弛
	*      ㄞ  弛    Up       ㄞ  弛
	*    ㄞ    弛           ㄞ	  弛
	*  ㄞ	   弛	   Ba ㄞ      弛
	  3	式式式式式式式式式 2      弛 
	  弛       弛        弛       弛
	  弛   L   弛        弛   R   弛
      弛      4  式式式式弛式式式 5
	  弛       ㄞFr      弛      ㄞ
	  弛     ㄞ          弛    ㄞ
	  弛   ㄞ      Bo    弛  ㄞ
	  弛 ㄞ              弛ㄞ
	  7 式式式式式式式式式 6

	
	*/

	//upper rectangle
	vecVertices[0].v3Vtx.x = -1;
	vecVertices[0].v3Vtx.y = 1;
	vecVertices[0].v3Vtx.z = 1;
	vecVertices[1].v3Vtx.x = 1;
	vecVertices[1].v3Vtx.y = 1;
	vecVertices[1].v3Vtx.z = 1;
	vecVertices[2].v3Vtx.x = 1;
	vecVertices[2].v3Vtx.y = 1;
	vecVertices[2].v3Vtx.z = -1;
	vecVertices[3].v3Vtx.x = -1;
	vecVertices[3].v3Vtx.y = 1;
	vecVertices[3].v3Vtx.z = -1;

	//bottom rectangle
	vecVertices[4].v3Vtx.x = -1;
	vecVertices[4].v3Vtx.y = -1;
	vecVertices[4].v3Vtx.z = 1;
	vecVertices[5].v3Vtx.x = 1;
	vecVertices[5].v3Vtx.y = -1;
	vecVertices[5].v3Vtx.z = 1;
	vecVertices[6].v3Vtx.x = 1;
	vecVertices[6].v3Vtx.y = -1;
	vecVertices[6].v3Vtx.z = -1;
	vecVertices[7].v3Vtx.x = -1;
	vecVertices[7].v3Vtx.y = -1;
	vecVertices[7].v3Vtx.z = -1;

	//triangle faces
	vecIndices.resize(3*2*6);

	//upper face
	vecIndices[0] = 0;
	vecIndices[1] = 1;
	vecIndices[2] = 3;
	vecIndices[3] = 1;
	vecIndices[4] = 2;
	vecIndices[5] = 3;


	//7    6

	//4    5

	//bottom face
	vecIndices[6] = 7;
	vecIndices[7] = 6;
	vecIndices[8] = 4;
	vecIndices[9] = 6;
	vecIndices[10] = 5;
	vecIndices[11] = 4;

	// 3    2

	// 7    6

	//front face
	vecIndices[12] = 3;
	vecIndices[13] = 2;
	vecIndices[14] = 7;
	vecIndices[15] = 2;
	vecIndices[16] = 6;
	vecIndices[17] = 7;


	// 4      5

	//  0      1

	//back face
	vecIndices[18] = 4;
	vecIndices[19] = 5;
	vecIndices[20] = 0;
	vecIndices[21] = 5;
	vecIndices[22] = 1;
	vecIndices[23] = 0;

	// 0  3

	//  4  7

	//left face
	vecIndices[24] = 0;
	vecIndices[25] = 3;
	vecIndices[26] = 4;
	vecIndices[27] = 3;
	vecIndices[28] = 7;
	vecIndices[29] = 4;

	//  2   1

	//  6   5
	//right face
	vecIndices[30] = 2;
	vecIndices[31] = 1;
	vecIndices[32] = 6;
	vecIndices[33] = 1;
	vecIndices[34] = 5;
	vecIndices[35] = 6;

	vecVertices[0].v4Color = XMFLOAT4(1, 0, 0, 1);
	vecVertices[1].v4Color = XMFLOAT4(0, 1, 0, 1);
	vecVertices[2].v4Color = XMFLOAT4(0, 0, 1, 1);
	vecVertices[3].v4Color = XMFLOAT4(1, 1, 1, 1);
	vecVertices[4].v4Color = XMFLOAT4(1, 0, 0, 1);
	vecVertices[5].v4Color = XMFLOAT4(0, 1, 0, 1);
	vecVertices[6].v4Color = XMFLOAT4(0, 0, 1, 1);
	vecVertices[7].v4Color = XMFLOAT4(1, 1, 1, 1);


	vecFacies.resize(vecIndices.size() / 3);

	//input face
	for (size_t i = 0; i < vecIndices.size(); i += 3)
	{
		CFace* pFace = &vecFacies[i / 3];
 
 		vecVertices[vecIndices[i + 0]].uiIndex = 0;
		vecVertices[vecIndices[i + 1]].uiIndex = 1;
		vecVertices[vecIndices[i + 2]].uiIndex = 2;
		
		pFace->SetFace(vecVertices[vecIndices[i + 0]], vecVertices[vecIndices[i + 1]], vecVertices[vecIndices[i + 2]]);
		pFace->MakeNormal();
	}
}

//
//void CCube::Initialize()
//{
//	m_vecModels.clear();
//	m_vecModels.push_back(MESH_MODEL());
//
//	MESH_MODEL* pModel = &m_vecModels.back();
//	pModel->strName = L"Plane";
//
//	vector<VERTEX>& vecVertices = pModel->vecVertices;
//	vector<CFace>& vecFacies = pModel->vecFacies;
//	vector<DWORD> vecIndices;
//	
//	vecVertices.resize(4);
//
//	/*
//	*       0                     1
//	*          式式式式式式式式式
//	*        ㄞ弛               ㄞ弛
//	*      ㄞ  弛    Up       ㄞ  弛
//	*    ㄞ    弛           ㄞ	  弛
//	*  ㄞ	   弛	   Ba ㄞ      弛
//	  3	式式式式式式式式式 2      弛
//	  弛       弛        弛       弛
//	  弛   L   弛        弛   R   弛
//	  弛      4  式式式式弛式式式 5
//	  弛       ㄞFr      弛      ㄞ
//	  弛     ㄞ          弛    ㄞ
//	  弛   ㄞ      Bo    弛  ㄞ
//	  弛 ㄞ              弛ㄞ
//	  7 式式式式式式式式式 6
//
//
//	*/
//
//	//upper rectangle
//	//vecVertices[0].v3Vtx.x = -1;
//	//vecVertices[0].v3Vtx.y = 1;
//	//vecVertices[0].v3Vtx.z = 1;
//	//vecVertices[1].v3Vtx.x = 1;
//	//vecVertices[1].v3Vtx.y = 1;
//	//vecVertices[1].v3Vtx.z = 1;
//	//vecVertices[2].v3Vtx.x = 1;
//	//vecVertices[2].v3Vtx.y = 1;
//	//vecVertices[2].v3Vtx.z = -1;
//	//vecVertices[3].v3Vtx.x = -1;
//	//vecVertices[3].v3Vtx.y = 1;
//	//vecVertices[3].v3Vtx.z = -1;
//
//	vecVertices[0].v3Vtx.x = -1;
//	vecVertices[0].v3Vtx.y = 1;
//	vecVertices[0].v3Vtx.z = 1;
//	vecVertices[1].v3Vtx.x = 1;
//	vecVertices[1].v3Vtx.y = 1;
//	vecVertices[1].v3Vtx.z = 1;
//	vecVertices[2].v3Vtx.x = 1;
//	vecVertices[2].v3Vtx.y = -1;
//	vecVertices[2].v3Vtx.z = 1;
//	vecVertices[3].v3Vtx.x = -1;
//	vecVertices[3].v3Vtx.y = -1;
//	vecVertices[3].v3Vtx.z = 1;
//
//	//vecVertices[0].v3Vtx.x = -1;
//	//vecVertices[0].v3Vtx.y = 1;
//	//vecVertices[0].v3Vtx.z = 1;
//	//vecVertices[1].v3Vtx.x = -1;
//	//vecVertices[1].v3Vtx.y = 1;
//	//vecVertices[1].v3Vtx.z = -1;
//	//vecVertices[2].v3Vtx.x = -1;
//	//vecVertices[2].v3Vtx.y = -1;
//	//vecVertices[2].v3Vtx.z = -1;
//	//vecVertices[3].v3Vtx.x = -1;
//	//vecVertices[3].v3Vtx.y = -1;
//	//vecVertices[3].v3Vtx.z = 1;
//
//
//	vecVertices[0].v4Color = XMFLOAT4(1,0,0,1);
//	vecVertices[1].v4Color = XMFLOAT4(0,1,0,1);
//	vecVertices[2].v4Color = XMFLOAT4(0,0,1,1);
//	vecVertices[3].v4Color = XMFLOAT4(1,1,1,1);
//
//	vecVertices[0].v2UV.x = 0;
//	vecVertices[0].v2UV.y = 1;
//	vecVertices[1].v2UV.x = 1;
//	vecVertices[1].v2UV.y = 1;
//	vecVertices[2].v2UV.x = 1;
//	vecVertices[2].v2UV.y = 0;
//	vecVertices[3].v2UV.x = 0;
//	vecVertices[3].v2UV.y = 0;
//
//
//	//triangle faces
//	vecIndices.resize(3 * 2);
//
//	//front face
//	vecIndices[0] = 0;
//	vecIndices[1] = 1;
//	vecIndices[2] = 3;
//	vecIndices[3] = 1;
//	vecIndices[4] = 2;
//	vecIndices[5] = 3;
//
//	vecFacies.resize(vecIndices.size() / 3);
//
//	//input face
//	for (size_t i = 0; i < vecIndices.size(); i += 3)
//	{
//		CFace* pFace = &vecFacies[i / 3];
//
//		vecVertices[vecIndices[i + 0]].uiIndex = 0;
//		vecVertices[vecIndices[i + 1]].uiIndex = 1;
//		vecVertices[vecIndices[i + 2]].uiIndex = 2;
//
//		pFace->SetMaterialName(L"Material");
//
//		pFace->SetFace(vecVertices[vecIndices[i + 0]], vecVertices[vecIndices[i + 1]], vecVertices[vecIndices[i + 2]]);
//		pFace->MakeNormal();
//	}
//}
