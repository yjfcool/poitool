// pane_mem.h: interface for the pane_mem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(PANE_MEM_H_)
#define PANE_MEM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "mem_block.h"
#include "geo_pane.h"

struct idx_pane
{
	unsigned int x_idx;
	unsigned int y_idx;
	b_uchar_mem  pane_mem;
};

class pane_mem_cache
{
public:
	pane_mem_cache():m_p_mem(NULL),m_x_idx(0),m_y_idx(0),m_x_dem(0),m_y_dem(0),m_geo_pane(0)
	{
	}
	virtual ~pane_mem_cache();
	BOOL need_build()
	{
		return m_geo_pane == NULL?TRUE:FALSE;
	}
	BOOL build_pane(unsigned int x_begin, unsigned int x_end, unsigned int y_begin,
					unsigned int y_end,   unsigned int x_pane_num, unsigned int y_pane_num)
	{
		if (m_geo_pane == NULL) 
		{
			m_geo_pane = new geo_pane(x_begin, x_end, y_begin, y_end, x_pane_num, y_pane_num);
			return m_geo_pane?TRUE:FALSE;
		}
		else
			return TRUE;
	}
	BOOL build_pane_mem(POINT pt, int radius);
	idx_pane& operator[](int idx)
	{
		return m_p_mem[idx / m_x_dem][idx % m_x_dem];
	}
	//方格左下角实际坐标
	BOOL pane_coord(int idx, int& x, int& y)
	{
		if (m_geo_pane == NULL) 
		{
			return FALSE;
		}
		return m_geo_pane->pane_coord(idx, x, y);
	}
	int mem_pane_num(void)const{return m_x_dem * m_y_dem;}
	int index(unsigned int x_idx, unsigned int y_idx)
	{
		if (m_geo_pane == NULL) 
		{
			return m_geo_pane->index(x_idx, y_idx);
		}
		return 0;
	}
	//------------------------------------------------------------------测试接口
	unsigned int x_begin(){return m_x_idx;}
	unsigned int y_begin(){return m_y_idx;}
	unsigned int x_dem(){return m_x_dem;}
	unsigned int y_dem(){return m_y_dem;}
private:
	friend class pane_mem_cache;
	unsigned int m_x_idx;
	unsigned int m_y_idx;
	unsigned int m_x_dem;
	unsigned int m_y_dem;
	idx_pane**	m_p_mem;
	geo_pane*	m_geo_pane;
};


#endif // !defined(AFX_PANE_MEM_H__B3DE86C8_93A1_4B3C_867C_A74C96FE2DC3__INCLUDED_)
