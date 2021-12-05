#include "pch.h"
#include "CBresenhamLine.h"

void CBresenhamLine::Initialize(const CPoint ptStart, const CPoint ptEnd)
{
	m_ptStart = ptStart;
	m_ptEnd = ptEnd;

	m_x = 0;
	m_y = 0;

	m_ix = 0;
	m_iy = 0;

	m_startX = 0;
	m_startY = 0;
	m_endX = 0;
	m_endY = 0;

	m_fEndRate = 0;
	m_lCnt = 0;
	m_fLen = 0;

	m_bFinish = false;

	if (abs(ptEnd.y - ptStart.y) < abs(ptEnd.x - ptStart.x))
	{
		m_bUpper = false;

		//end to start
		m_fLen = abs(ptStart.x - (float)ptEnd.x) - 1;
		if (m_fLen < 0) m_fLen = 0;

		if (ptStart.x > ptEnd.x)
		{
			m_bLeft = true;

			m_dx = ptStart.x - ptEnd.x;
			m_dy = ptStart.y - ptEnd.y;

			m_x = ptEnd.x;
			m_startX = ptEnd.x;
			m_endX = ptStart.x;

			m_y = ptEnd.y;

		}
		//start to end
		else
		{
			m_bLeft = false;

			m_dx = ptEnd.x - ptStart.x;
			m_dy = ptEnd.y - ptStart.y;

			m_x = ptStart.x;
			m_startX = ptStart.x;
			m_endX = ptEnd.x;

			m_y = ptStart.y;

		}

		m_iy = 1;
		if (m_dy < 0)
		{
			m_iy = -1;
			m_dy = -m_dy;
		}

		m_D = (m_dy << 1) - m_dx;


	}
	else
	{
		m_bUpper = true;

		//end to start
		m_fLen = abs(ptStart.y - (float)ptEnd.y) - 1;
		if (m_fLen < 0)
			m_fLen;

		if (ptStart.y > ptEnd.y)
		{
			m_bLeft = true;

			m_dx = ptStart.x - ptEnd.x;
			m_dy = ptStart.y - ptEnd.y;

			m_y = ptEnd.y;
			m_startY = ptEnd.y;
			m_endY = ptStart.y;

			m_x = ptEnd.x;
		}
		//start to end
		else
		{
			m_bLeft = false;

			m_dx = ptEnd.x - ptStart.x;
			m_dy = ptEnd.y - ptStart.y;

			m_y = ptStart.y;
			m_startY = ptStart.y;
			m_endY = ptEnd.y;

			m_x = ptStart.x;
		}

		m_ix = 1;
		if (m_dx < 0)
		{
			m_ix = -1;
			m_dx = -m_dx;
		}

		m_D = (m_dx << 1) - m_dy;
	}
}

long CBresenhamLine::GetStartX() const
{
	return m_startX;
}

long CBresenhamLine::GetStartY() const
{
	return m_startY;
}

long CBresenhamLine::GetEndX() const
{
	return m_endX;
}

long CBresenhamLine::GetEndY() const
{
	return m_endY;
}

long CBresenhamLine::GetCurrentY() const
{
	return m_y;
}

long CBresenhamLine::GetInvY()
{
	return m_endY + (m_startY - m_y);
}

long CBresenhamLine::GetCurrentX() const
{
	return m_x;
}

long CBresenhamLine::GetInvX()
{
	return m_endX + (m_startX - m_y);
}

float CBresenhamLine::GetCurrentRate() const
{
	return m_fEndRate;
}

bool CBresenhamLine::isUpper() const
{
	return m_bUpper;
}

bool CBresenhamLine::isLeft() const
{
	return m_bLeft;
}

bool CBresenhamLine::isFinish() const
{
	return m_bFinish;
}

bool CBresenhamLine::MoveNext()
{
	if (m_bUpper == false)
	{
		if (m_x >= m_endX)
		{
			m_bFinish = true;
			return false;
		}

		m_fEndRate = m_lCnt++ / m_fLen;

		m_x++;


		if (m_D > 0)
		{
			m_y += m_iy;
			m_D = m_D + ((m_dy - m_dx) << 1);
		}
		else
			m_D = m_D + (m_dy << 1);

	}
	else
	{
		if (m_y >= m_endY)
		{
			m_bFinish = true;
			return false;
		}

		m_fEndRate = m_lCnt++ / m_fLen;
		m_y++;


		if (m_D > 0)
		{
			m_x += m_ix;
			m_D = m_D + ((m_dx - m_dy) << 1);
		}
		else
			m_D = m_D + (m_dx << 1);


	}



	return true;
}

