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

void CCube::Initialize()
{
	m_vecVertices.clear();
	m_vecIndices.clear();

	m_vecVertices.resize(8);

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
	m_vecVertices[0].x = -1;
	m_vecVertices[0].y = 1;
	m_vecVertices[0].z = 1;
	m_vecVertices[1].x = 1;
	m_vecVertices[1].y = 1;
	m_vecVertices[1].z = 1;
	m_vecVertices[2].x = 1;
	m_vecVertices[2].y = 1;
	m_vecVertices[2].z = -1;
	m_vecVertices[3].x = -1;
	m_vecVertices[3].y = 1;
	m_vecVertices[3].z = -1;

	//bottom rectangle
	m_vecVertices[4].x = -1;
	m_vecVertices[4].y = -1;
	m_vecVertices[4].z = 1;
	m_vecVertices[5].x = 1;
	m_vecVertices[5].y = -1;
	m_vecVertices[5].z = 1;
	m_vecVertices[6].x = 1;
	m_vecVertices[6].y = -1;
	m_vecVertices[6].z = -1;
	m_vecVertices[7].x = -1;
	m_vecVertices[7].y = -1;
	m_vecVertices[7].z = -1;

	//triangle faces
	m_vecIndices.resize(3*2*6);

	//upper face
	m_vecIndices[0] = 0;
	m_vecIndices[1] = 1;
	m_vecIndices[2] = 3;
	m_vecIndices[3] = 1;
	m_vecIndices[4] = 2;
	m_vecIndices[5] = 3;


	//7    6

	//4    5

	//bottom face
	m_vecIndices[6] = 7;
	m_vecIndices[7] = 6;
	m_vecIndices[8] = 4;
	m_vecIndices[9] = 6;
	m_vecIndices[10] = 5;
	m_vecIndices[11] = 4;

	// 3    2

	// 7    6

	//front face
	m_vecIndices[12] = 3;
	m_vecIndices[13] = 2;
	m_vecIndices[14] = 7;
	m_vecIndices[15] = 2;
	m_vecIndices[16] = 6;
	m_vecIndices[17] = 7;


	// 4      5

	//  0      1

	//back face
	m_vecIndices[18] = 4;
	m_vecIndices[19] = 5;
	m_vecIndices[20] = 0;
	m_vecIndices[21] = 5;
	m_vecIndices[22] = 1;
	m_vecIndices[23] = 0;

	// 0  3

	//  4  7

	//left face
	m_vecIndices[24] = 0;
	m_vecIndices[25] = 3;
	m_vecIndices[26] = 4;
	m_vecIndices[27] = 3;
	m_vecIndices[28] = 7;
	m_vecIndices[29] = 4;

	//  2   1

	//  6   5
	//right face
	m_vecIndices[30] = 2;
	m_vecIndices[31] = 1;
	m_vecIndices[32] = 6;
	m_vecIndices[33] = 1;
	m_vecIndices[34] = 5;
	m_vecIndices[35] = 6;

}
