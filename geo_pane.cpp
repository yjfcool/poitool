#include "stdafx.h"
#include "geo_pane.h"

/*
	pt	 -	焦点 
	radius  以pt为圆心的半径
			pt 和radius 形成一个矩形范围bnd
	x_idx - bnd  左下角点x索引 
	y_idx - bnd  左下角点y索引
	x_dem - 矩形x方向所占的方格数目
	y_dem - 
 *	左下角是索引是x_idx = 0, y_idx = 0
 */
BOOL geo_pane::bnd_idx(POINT pt, unsigned radius, unsigned& x_idx, unsigned& y_idx, 
													unsigned& x_dem, unsigned& y_dem)
{
	//---------------------------------------------------------------------------------------边界交集
	POINT ld, ru;
	ld.x = max((int)(pt.x - radius), (int)m_x_begin);
	ld.y = max((int)(pt.y - radius), (int)m_y_begin);
	ru.x = min((int)(pt.x + radius), (int)m_x_end);
	ru.y = min((int)(pt.y + radius), (int)m_y_end);
	//----------------------------------------------------------------------------------------超出范围
	if(ru.x <= m_x_begin || ld.x >= m_x_end || ru.y <= m_y_begin || ld.y >= m_y_end)
		return FALSE;
	//----------------------------------------------------------------------------------------左下角索引
	x_idx = (ld.x - m_x_begin) / m_pane_width;
	y_idx = (ld.y - m_y_begin) / m_pane_height;
	//----------------------------------------------------------------------------------------索引偏移量
	unsigned int x_end = ((ru.x % m_pane_width) == 0)?(ru.x / m_pane_width):(ru.x / m_pane_width + 1);
	unsigned int y_end = ((ru.y % m_pane_height) == 0)?(ru.x / m_pane_height):(ru.x / m_pane_height + 1);
	x_dem = x_end - x_idx;
	y_dem = y_end - y_idx;
	return TRUE;
}
/*
 *	没有越界判断
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
