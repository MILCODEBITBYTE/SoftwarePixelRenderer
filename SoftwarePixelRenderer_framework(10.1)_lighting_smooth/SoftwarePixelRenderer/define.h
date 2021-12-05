#pragma once
#define dfRENDERTARGET() g_pD2dSystem->GetRenderTarget()
#define dfWRITEFACTORY() g_pD2dSystem->GetWriteFactory()
#define dfBMPRENDERTARGET() g_pD2dSystem->GetWicRenderTarget()

#define dfCAMERA() g_pCamera

template <typename T>
void gf_SafeRelease(T p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}


