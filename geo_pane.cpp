#include "stdafx.h"
#include "geo_pane.h"

/*
	pt	 -	���� 
	radius  ��ptΪԲ�ĵİ뾶
			pt ��radius �γ�һ�����η�Χbnd
	x_idx - bnd  ���½ǵ�x���� 
	y_idx - bnd  ���½ǵ�y����
	x_dem - ����x������ռ�ķ�����Ŀ
	y_dem - 
 *	���½���������x_idx = 0, y_idx = 0
 */
BOOL geo_pane::bnd_idx(POINT pt, unsigned radius, unsigned& x_idx, unsigned& y_idx, 
													unsigned& x_dem, unsigned& y_dem)
{
	//---------------------------------------------------------------------------------------�߽罻��
	POINT ld, ru;
	ld.x = max((int)(pt.x - radius), (int)m_x_begin);
	ld.y = max((int)(pt.y - radius), (int)m_y_begin);
	ru.x = min((int)(pt.x + radius), (int)m_x_end);
	ru.y = min((int)(pt.y + radius), (int)m_y_end);
	//----------------------------------------------------------------------------------------������Χ
	if(ru.x <= m_x_begin || ld.x >= m_x_end || ru.y <= m_y_begin || ld.y >= m_y_end)
		return FALSE;
	//----------------------------------------------------------------------------------------���½�����
	x_idx = (ld.x - m_x_begin) / m_pane_width;
	y_idx = (ld.y - m_y_begin) / m_pane_height;
	//----------------------------------------------------------------------------------------����ƫ����
	unsigned int x_end = ((ru.x % m_pane_width) == 0)?(ru.x / m_pane_width):(ru.x / m_pane_width + 1);
	unsigned int y_end = ((ru.y % m_pane_height) == 0)?(ru.x / m_pane_height):(ru.x / m_pane_height + 1);
	x_dem = x_end - x_idx;
	y_dem = y_end - y_idx;
	return TRUE;
}
/*
 *	û��Խ���ж�
 */
int geo_pane::index(int x_idx, int y_idx)
{
	return x_idx + y_idx * m_x_pane_num;
}

BOOL geo_pane::pane_coord(int idx, int& x, int& y)
{
	if (idx > (m_x_pane_num * m_y_pane_num)) 
		return FALSE;

	x = idx % m_x_pane_num * m_pane_width;
	y = idx / m_x_pane_num * m_pane_height;
	return TRUE;
}
