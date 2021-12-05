#pragma once

#include <DirectXMath.h>
using namespace DirectX;

typedef struct _tagVertex
{
	XMFLOAT3	v3Vtx;
	XMFLOAT3	v3Normal;
	XMFLOAT2	v2UV;
	XMFLOAT4	v4Color;
	UINT		uiIndex;
}VERTEX;
