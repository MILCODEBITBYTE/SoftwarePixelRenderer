// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일

#include "pch.h"

// 미리 컴파일된 헤더를 사용하는 경우 컴파일이 성공하려면 이 소스 파일이 필요합니다.
CD2dSystem* g_pD2dSystem = nullptr;
CCamera* g_pCamera[3] = { nullptr, };
vector<float> g_vecZBuffer;