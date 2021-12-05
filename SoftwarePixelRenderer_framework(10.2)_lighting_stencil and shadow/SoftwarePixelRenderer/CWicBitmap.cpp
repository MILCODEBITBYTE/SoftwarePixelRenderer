#include "PCH.h"
#include "CWicBitmap.h"

CWicBitmap::CWicBitmap()
{
	m_pWicSrc = nullptr;
	m_pD2dSrc = nullptr;
	m_bAlpha = false;
}

CWicBitmap::~CWicBitmap()
{

}


void CWicBitmap::ShutdownResource()
{
	if (m_pWicSrc)
	{
		m_pWicSrc->Release();
		m_pWicSrc = nullptr;
	}
	if (m_pD2dSrc)
	{
		m_pD2dSrc->Release();
		m_pD2dSrc = nullptr;
	}

	m_bAlpha = false;
}


BOOL CWicBitmap::LoadWicBitmap(ID2D1RenderTarget* pRender, LPCTSTR szFile)
{
	ShutdownResource();

	BOOL bRes = TRUE;

	HRESULT hr;

	IWICImagingFactory* pFactory = nullptr;
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pFrame = nullptr;
	IWICFormatConverter* pConverter = nullptr;

	hr = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));

	if (FAILED(hr))
	{
		return FALSE;
	}

	hr = pFactory->CreateDecoderFromFilename(szFile, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

	if (FAILED(hr))
	{
		bRes = FALSE;
		goto _finall;
	}

	hr = pDecoder->GetFrame(0, &pFrame);
	if (FAILED(hr))
	{
		bRes = FALSE;
		goto _finall;
	}

	UINT uiWidth, uiHeight;
	pFrame->GetSize(&uiWidth, &uiHeight);

	hr = pFactory->CreateBitmapFromSourceRect(pFrame, 0, 0, uiWidth, uiHeight, &m_pWicSrc);
	if (FAILED(hr))
	{
		bRes = FALSE;
		goto _finall;
	}

	hr = pFactory->CreateFormatConverter(&pConverter);

	if (FAILED(hr))
	{
		bRes = FALSE;
		goto _finall;
	}

	WICPixelFormatGUID guid, target;


	pFrame->GetPixelFormat(&guid);

	SetPixelFormat(guid);

	if (GUID_WICPixelFormat32bppPBGRA == guid || GUID_WICPixelFormat32bppPRGBA == guid)
	{
		target = GUID_WICPixelFormat32bppPBGRA;
	}
	else if (GUID_WICPixelFormat32bppBGRA == guid || GUID_WICPixelFormat32bppRGBA == guid)
	{
		target = GUID_WICPixelFormat32bppBGRA;
	}
	else
	{
		target = GUID_WICPixelFormat32bppPBGRA;
	}


	hr = pConverter->Initialize(pFrame, target, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeCustom);
	if (FAILED(hr))
	{
		bRes = FALSE;
		goto _finall;
	}

	hr = pRender->CreateBitmapFromWicBitmap(pConverter, &m_pD2dSrc);
	if (FAILED(hr))
	{
		bRes = FALSE;
		goto _finall;
	}


	

_finall:
	
	gf_SafeRelease(pFactory);
	gf_SafeRelease(pDecoder);
	gf_SafeRelease(pFrame);
	gf_SafeRelease(pConverter);

	return bRes;
}

BOOL CWicBitmap::CreateBitmap(UINT uiWidth, UINT uiHeight)
{
	ShutdownResource();

	BOOL bRes = TRUE;
	IWICImagingFactory* pWicFactory = nullptr;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWicFactory));
	if (FAILED(hr))
		return FALSE;


	SetPixelFormat(GUID_WICPixelFormat32bppPBGRA);

	hr = pWicFactory->CreateBitmap(uiWidth, uiHeight, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &m_pWicSrc);
	if (FAILED(hr))
	{
		bRes = FALSE;
	}

	if(pWicFactory)
		pWicFactory->Release();

	return bRes;
}

BOOL CWicBitmap::ConvertWicToD2dBitmap(ID2D1RenderTarget *pRender)
{
	BOOL bRes = TRUE;
	if (m_pWicSrc == nullptr|| pRender == nullptr) return FALSE;
	if (m_pD2dSrc)
		m_pD2dSrc->Release();
	m_pD2dSrc = nullptr;

	IWICImagingFactory* pWicFactory = nullptr;
	IWICFormatConverter* pConverter = nullptr;

	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWicFactory));

	if (FAILED(hr))
		return FALSE;


	hr = pWicFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr))
	{
		bRes = FALSE;
		goto finally__;
	}

	WICPixelFormatGUID guid, target;

	if (GUID_WICPixelFormat32bppPBGRA == guid || GUID_WICPixelFormat32bppPRGBA == guid)
	{
		target = GUID_WICPixelFormat32bppPBGRA;
	}
	else if (GUID_WICPixelFormat32bppBGRA == guid || GUID_WICPixelFormat32bppRGBA == guid)
	{
		target = GUID_WICPixelFormat32bppBGRA;
	}
	else
	{
		target = GUID_WICPixelFormat32bppPBGRA;
	}

	hr = pConverter->Initialize(m_pWicSrc, target, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);
	if (FAILED(hr))
	{
		bRes = FALSE;
		goto finally__;
	}

	hr = pRender->CreateBitmapFromWicBitmap(pConverter, NULL, &m_pD2dSrc);
	if (FAILED(hr))
	{
		bRes = FALSE;
		goto finally__;
	}

finally__:

	if (pConverter)
		pConverter->Release();

	if (pWicFactory)
		pWicFactory->Release();

	return bRes;
}

ID2D1Bitmap* CWicBitmap::ConvertWicToD2dBitmap(ID2D1RenderTarget* pRender, IWICBitmap* pSrc)
{
	BOOL bRes = TRUE;
	if (pRender == nullptr || pSrc == nullptr) return FALSE;

	ID2D1Bitmap* pBmp = nullptr;

	IWICImagingFactory* pWicFactory = nullptr;
	IWICFormatConverter* pConverter = nullptr;

	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWicFactory));

	if (FAILED(hr))
		return FALSE;


	hr = pWicFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr))
	{
		bRes = FALSE;
		goto finally__;
	}

	WICPixelFormatGUID guid, target;

	if (GUID_WICPixelFormat32bppPBGRA == guid || GUID_WICPixelFormat32bppPRGBA == guid)
	{
		target = GUID_WICPixelFormat32bppPBGRA;
	}
	else if (GUID_WICPixelFormat32bppBGRA == guid || GUID_WICPixelFormat32bppRGBA == guid)
	{
		target = GUID_WICPixelFormat32bppBGRA;
	}
	else
	{
		target = GUID_WICPixelFormat32bppPBGRA;
	}

	hr = pConverter->Initialize(pSrc, target, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);
	if (FAILED(hr))
	{
		bRes = FALSE;
		goto finally__;
	}

	hr = pRender->CreateBitmapFromWicBitmap(pConverter, NULL, &pBmp);
	if (FAILED(hr))
	{
		bRes = FALSE;
		goto finally__;
	}

finally__:

	if (pConverter)
		pConverter->Release();

	if (pWicFactory)
		pWicFactory->Release();

	return pBmp;
}

BOOL CWicBitmap::IsAlpha() const
{
	return m_bAlpha;
}

IWICBitmap* CWicBitmap::GetWicBitmap()
{
	return m_pWicSrc;
}

ID2D1Bitmap* CWicBitmap::GetD2dBitmap()
{
	return m_pD2dSrc;
}

int CWicBitmap::GetChannel() const
{
	return m_iChannel;
}

int CWicBitmap::GetBits() const
{
	return m_iBits;
}

void CWicBitmap::SetPixelFormat(const WICPixelFormatGUID& guid)
{
	m_guidSrc = guid;
	m_bAlpha = false;

	if (guid == GUID_WICPixelFormatBlackWhite)
	{
		m_iChannel = 1;
		m_iBits = 1;
	}
	else if (guid == GUID_WICPixelFormat2bppGray)
	{
		m_iChannel = 1;
		m_iBits = 2;
	}
	else if (guid == GUID_WICPixelFormat4bppGray)
	{
		m_iChannel = 1;
		m_iBits = 4;
	}
	else if (guid == GUID_WICPixelFormat8bppGray)
	{
		m_iChannel = 1;
		m_iBits = 8;
	}
	else if (guid == GUID_WICPixelFormat16bppGray)
	{
		m_iChannel = 1;
		m_iBits = 16;
	}
	else if (guid == GUID_WICPixelFormat24bppRGB)
	{
		m_iChannel = 3;
		m_iBits = 24;
		
	}
	else if (guid == GUID_WICPixelFormat24bppBGR)
	{
		m_iChannel = 3;
		m_iBits = 24;
	}
	else if (guid == GUID_WICPixelFormat32bppBGR)
	{
		m_iChannel = 3;
		m_iBits = 32;
	}
	else if (guid == GUID_WICPixelFormat32bppRGBA)
	{
		m_iChannel = 4;
		m_iBits = 32;
		m_bAlpha = true;
	}
	else if (guid == GUID_WICPixelFormat32bppBGRA)
	{
		m_iChannel = 4;
		m_iBits = 32;
		m_bAlpha = true;
	}
	else if (guid == GUID_WICPixelFormat32bppRGBE)
	{
		m_iChannel = 4;
		m_iBits = 32;
	}
	else if (guid == GUID_WICPixelFormat32bppPRGBA)
	{
		m_iChannel = 4;
		m_iBits = 32;
		m_bAlpha = true;
	}
	else if (guid == GUID_WICPixelFormat32bppPBGRA)
	{
		m_iChannel = 4;
		m_iBits = 32;
		m_bAlpha = true;
	}
	else
	{
		m_guidSrc = GUID_WICPixelFormat24bppBGR;
		m_iChannel = 3;
		m_iBits = 24;
	}
}
