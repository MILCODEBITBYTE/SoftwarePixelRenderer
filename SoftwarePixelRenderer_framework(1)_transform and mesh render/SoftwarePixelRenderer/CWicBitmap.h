#pragma once



class CWicBitmap
{
public :
	CWicBitmap();
	virtual ~CWicBitmap();

	void ShutdownResource();


	BOOL LoadWicBitmap(ID2D1RenderTarget* pRender, LPCTSTR szFile);
	BOOL CreateBitmap(UINT uiWidth, UINT uiHeight);
	BOOL ConvertWicToD2dBitmap(ID2D1RenderTarget *pRender);
	static ID2D1Bitmap* ConvertWicToD2dBitmap(ID2D1RenderTarget* pRender, IWICBitmap* pSrc);
	BOOL IsAlpha() const;

	IWICBitmap* GetWicBitmap();
	ID2D1Bitmap* GetD2dBitmap();

	int GetChannel() const;
	int GetBits() const;

protected:

	void SetPixelFormat(const WICPixelFormatGUID& guid);

private:
	IWICBitmap* m_pWicSrc;
	ID2D1Bitmap* m_pD2dSrc;

	WICPixelFormatGUID m_guidSrc;
	bool m_bAlpha;
	int m_iChannel;
	int m_iBits;
};

