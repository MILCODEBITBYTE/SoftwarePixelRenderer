#pragma once

class CD2dSystem
{
public:
	CD2dSystem();
	bool Initialize(HWND hWnd);
	bool Initialize(HWND hWnd, const int iBufferWidth, const int iBufferHeight);
	void Shutdown();


	bool ResetRenderTarget(HWND hWnd, int cx, int cy);
	bool ResizeRenderTarget(int cx, int cy);
	bool ResizeBuffer(int cx, int cy);

	ID2D1HwndRenderTarget* GetRenderTarget() const;
	IDWriteFactory*			GetWriteFactory() const;
	ID2D1BitmapRenderTarget* GetBmpRenderTarget() const;
	ID2D1RenderTarget* GetWicRenderTarget() const;
	IWICBitmap* GetBackbuffer();
	IWICBitmap* GetRenderbuffer();
	void ClearScreen(CONST D2D1_COLOR_F& clearColor);
	void SwapBuffer();

private:
	ID2D1Factory*				m_ipFactory;
	ID2D1HwndRenderTarget*		m_ipRenderTarget;
	IDWriteFactory*				m_ipWriteFactory;
	ID2D1BitmapRenderTarget*	m_ipBmpRenderTarget;
	ID2D1RenderTarget*			m_ipWicRenderTarget[2];

	IWICBitmap* m_pBack[2];

	int m_iSwapIndex;

	int m_iBufferWidth;
	int m_iBufferHeight;

};

