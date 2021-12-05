#pragma once

class CBresenhamLine
{
public:
	void Initialize(const CPoint ptStart, const CPoint ptEnd);
	long GetStartX() const;
	long GetStartY() const;
	long GetEndX() const;
	long GetEndY() const;
	long GetCurrentY() const;
	long GetInvY();
	long GetCurrentX() const;
	long GetInvX();
	float GetCurrentRate() const;
	bool isUpper() const;
	bool isLeft() const;
	bool isFinish() const;
	// false is end;
	bool MoveNext();

private:
	CPoint m_ptStart;
	CPoint m_ptEnd;


	long m_x;
	long m_y;

	long m_startX;
	long m_startY;
	long m_endX;
	long m_endY;
	float m_fEndRate;

	bool m_bUpper;
	bool m_bLeft;

	long m_dx, m_dy;
	long m_ix, m_iy;
	long m_D;

	long m_lCnt;
	float m_fLen;
	bool m_bFinish;
};

