#include "PCH.h"
#include "CD2dSystem.h"

using namespace D2D1;

CD2dSystem::CD2dSystem()
{
	m_ipFactory = nullptr;
	m_ipRenderTarget = nullptr;
	m_ipWriteFactory = nullptr;
	m_ipBmpRenderTarget = nullptr;
	m_ipWicRenderTarget[0] = m_ipWicRenderTarget[1] = nullptr;

	m_pBack[0] = m_pBack[1] = nullptr;
	m_iBufferWidth = 0;
	m_iBufferHeight = 0;
	m_iSwapIndex = 0;

}

CD2dSystem::~CD2dSystem()
{
}

bool CD2dSystem::Initialize(HWND hWnd)
{
	CRect rc;
	GetClientRect(hWnd, &rc);



	return Initialize(hWnd, rc.Width(), rc.Height());
}

bool CD2dSystem::Initialize(HWND hWnd, const int iBufferWidth, const int iBufferHeight)
{
	HRESULT hr;

	hr = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_ipFactory);
	if (FAILED(hr))
	{
		printf("Failed to Create D2DFactory..\r\n");
		return false;
	}

	CRect rc;
	GetClientRect(hWnd, &rc);

	D2D1_SIZE_U size = SizeU(rc.Width(), rc.Height());

	D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
	props.pixelFormat = pixelFormat;

	hr = m_ipFactory->CreateHwndRenderTarget(props,
		D2D1::HwndRenderTargetProperties(hWnd, size),
		&m_ipRenderTarget);


	if (FAILED(hr))
	{
		printf("Failed to Create HwndRenderTarget..\r\n");
		return false;
	}

	D2D1_SIZE_F screen = D2D1::SizeF((float)rc.Width(), (float)rc.Height());

	hr = m_ipRenderTarget->CreateCompatibleRenderTarget(screen, &m_ipBmpRenderTarget);
	if (FAILED(hr))
	{
		if (FAILED(hr))
		{
			printf("Failed to Create BitmapRenderTarget..\r\n");
			return false;
		}
	}

	IWICImagingFactory* ipImgFactory;
	hr = ::CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ipImgFactory));

	if (FAILED(hr)) return false;

	m_iBufferWidth = iBufferWidth;
	m_iBufferHeight = iBufferHeight;

	for (int i = 0; i < 2; ++i)
	{
		hr = ipImgFactory->CreateBitmap(iBufferWidth, iBufferHeight, GUID_WICPixelFormat32bppPBGRA,
			WICBitmapCacheOnDemand, &m_pBack[i]);
		if (FAILED(hr))
			return false;



		hr = m_ipFactory->CreateWicBitmapRenderTarget(m_pBack[i], props, &m_ipWicRenderTarget[i]);
		if (FAILED(hr))
			return false;
	}





	ipImgFactory->Release();


	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(m_ipWriteFactory),
		reinterpret_cast<IUnknown**>(&m_ipWriteFactory));

	if (FAILED(hr))
	{
		printf("Failed to Create DWriteFactory..\r\n");
		return false;
	}

	return true;
}

void CD2dSystem::Shutdown()
{
	gf_SafeRelease(m_pBack[0]);
	gf_SafeRelease(m_pBack[1]);
	gf_SafeRelease(m_ipFactory);
	gf_SafeRelease(m_ipRenderTarget);
	gf_SafeRelease(m_ipWriteFactory);
	gf_SafeRelease(m_ipBmpRenderTarget);
	gf_SafeRelease(m_ipWicRenderTarget[0]);
	gf_SafeRelease(m_ipWicRenderTarget[1]);
}

bool CD2dSystem::ResetRenderTarget(HWND hWnd, int cx, int cy)
{
	HRESULT hr;

	if (m_ipFactory == nullptr) return false;

	D2D1_SIZE_U size = SizeU(cx, cy);

	D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
	props.pixelFormat = pixelFormat;

	hr = m_ipFactory->CreateHwndRenderTarget(props,
											D2D1::HwndRenderTargetProperties(hWnd, size),
											&m_ipRenderTarget);


	if (FAILED(hr))
	{
		printf("Failed to Create HwndRenderTarget..\r\n");
		return false;
	}

	D2D1_SIZE_F screen = D2D1::SizeF((float)cx, (float)cy);
	
	gf_SafeRelease(m_ipBmpRenderTarget);

	hr = m_ipRenderTarget->CreateCompatibleRenderTarget(screen, &m_ipBmpRenderTarget);
	if (FAILED(hr))
	{
		if (FAILED(hr))
		{
			printf("Failed to Create BitmapRenderTarget..\r\n");
			return false;
		}
	}

	IWICImagingFactory* ipImgFactory;
	hr = ::CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ipImgFactory));
	if (FAILED(hr)) return false;

	if (m_iBufferWidth == 0 && m_iBufferHeight == 0)
	{
		m_iBufferWidth = cx;
		m_iBufferHeight = cy;
	}

	for (int i = 0; i < 2; ++i)
	{
		gf_SafeRelease(m_pBack[i]);

		hr = ipImgFactory->CreateBitmap(m_iBufferWidth, m_iBufferHeight, GUID_WICPixelFormat32bppPBGRA,
			WICBitmapCacheOnDemand, &m_pBack[i]);
		if (FAILED(hr))
			return false;

		gf_SafeRelease(m_ipWicRenderTarget[i]);

		hr = m_ipFactory->CreateWicBitmapRenderTarget(m_pBack[i], props, &m_ipWicRenderTarget[i]);
		if (FAILED(hr))
			return false;
	}


	ipImgFactory->Release();

	return true;
}

bool CD2dSystem::ResizeRenderTarget(int cx, int cy)
{
	HRESULT hr;

	if (m_ipRenderTarget == nullptr) return false;


	D2D1_SIZE_U size = SizeU(cx, cy);

	hr = m_ipRenderTarget->Resize(size);

	if (FAILED(hr))
	{
		printf("Failed to resize HwndRenderTarget..\r\n");
		return false;
	}

	D2D1_SIZE_F screen = D2D1::SizeF((float)cx, (float)cy);

	gf_SafeRelease(m_ipBmpRenderTarget);

	hr = m_ipRenderTarget->CreateCompatibleRenderTarget(screen, &m_ipBmpRenderTarget);
	if (FAILED(hr))
	{
		if (FAILED(hr))
		{
			printf("Failed to Create BitmapRenderTarget..\r\n");
			return false;
		}
	}
	return true;
}

bool CD2dSystem::ResizeBuffer(int cx, int cy)
{
	HRESULT hr;

	if (m_ipRenderTarget == nullptr) return false;


	D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
	props.pixelFormat = pixelFormat;

	IWICImagingFactory* ipImgFactory;
	hr = ::CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ipImgFactory));
	if (FAILED(hr)) return false;

	m_iBufferWidth = cx;
	m_iBufferHeight = cy;

	for (int i = 0; i < 2; ++i)
	{
		gf_SafeRelease(m_pBack[i]);

		hr = ipImgFactory->CreateBitmap(cx, cy, GUID_WICPixelFormat32bppPBGRA,
			WICBitmapCacheOnDemand, &m_pBack[i]);
		if (FAILED(hr))
			return false;

		gf_SafeRelease(m_ipWicRenderTarget[i]);

		hr = m_ipFactory->CreateWicBitmapRenderTarget(m_pBack[i], props, &m_ipWicRenderTarget[i]);
		if (FAILED(hr))
			return false;
	}



	ipImgFactory->Release();

	return true;
}


ID2D1HwndRenderTarget* CD2dSystem::GetRenderTarget() const
{

	return m_ipRenderTarget;;
}

IDWriteFactory* CD2dSystem::GetWriteFactory() const
{
	return m_ipWriteFactory;
}

ID2D1BitmapRenderTarget* CD2dSystem::GetBmpRenderTarget() const
{
	return m_ipBmpRenderTarget;
}

ID2D1RenderTarget* CD2dSystem::GetWicRenderTarget()
{
	ID2D1RenderTarget* pTarget;
	pTarget = m_ipWicRenderTarget[m_iSwapIndex];
	return pTarget;
}

IWICBitmap* CD2dSystem::GetBackbuffer()
{
	IWICBitmap* pBuffer;
	pBuffer = m_pBack[(m_iSwapIndex + 1) % 2];
	return pBuffer;
}

IWICBitmap* CD2dSystem::GetRenderbuffer()
{
	IWICBitmap* pBuffer;
	pBuffer = m_pBack[m_iSwapIndex];
	return pBuffer;
}

void CD2dSystem::ClearScreen(const D2D1_COLOR_F& clearColor)
{
	for (int i = 0; i < 2; ++i)
	{
		m_ipWicRenderTarget[i]->BeginDraw();
		m_ipWicRenderTarget[i]->Clear(clearColor);
		m_ipWicRenderTarget[i]->EndDraw();
	}
}

void CD2dSystem::SwapBuffer()
{
	m_iSwapIndex = (m_iSwapIndex + 1) % 2;
}
